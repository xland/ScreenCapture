#include "pch.h"
#include <include/Ling.h>
#include "Setting.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "App.h"
#include <fstream>

namespace {
    std::unique_ptr<Setting> setting;
    constexpr UINT capShortcutMsgId{ 100 };
    constexpr UINT longShortcutMsgId{ 101 };
    constexpr UINT videoShortcutMsgId{ 102 };
    // 配置文件的默认内容。空文件、坏 JSON、缺键都拿它兜底，所以这里列出的每一项
    // 都是代码里会直接按名字取的（见 getLang / getAutoStart / initShortcutKeys）
    constexpr std::wstring_view defaultConfig{ LR"""({"common":{"autoStart":false,"language":"zh-CN"},"shortcutKey":{"cap":"Ctrl+Alt+A","long":"","video":""}})""" };

    UINT shortcutMsgId(const std::wstring& type)
    {
        if (type == L"cap") return capShortcutMsgId;
        if (type == L"long") return longShortcutMsgId;
        if (type == L"video") return videoShortcutMsgId;
        return 0;
    }

    // 自己读文件而不用 Ling::Util::readFile：那个只认 UTF-16LE，而且是按字节数 resize 的，
    // 后面会跟着一半长度的 \0。用户拿记事本新建的 config.json 默认是 UTF-8，
    // 得认出来 —— 不然就会被当成坏文件，把人家写的配置直接盖掉
    std::wstring readConfigText(const std::filesystem::path& path)
    {
        std::ifstream file{ path, std::ios::binary };
        if (!file) return L"";
        std::string bytes{ std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{} };
        // UTF-16LE（save() 写出来的就是这个）：跳过 BOM，按 wchar_t 重新解释。
        // 字节数是奇数说明文件本来就坏了，末尾那半个字符丢掉，后面解析失败会走默认值
        if (bytes.size() >= 2 && static_cast<unsigned char>(bytes[0]) == 0xFF
            && static_cast<unsigned char>(bytes[1]) == 0xFE) {
            std::wstring str((bytes.size() - 2) / sizeof(wchar_t), L'\0');
            memcpy(str.data(), bytes.data() + 2, str.size() * sizeof(wchar_t));
            return str;
        }
        if (bytes.starts_with("\xEF\xBB\xBF")) bytes.erase(0, 3); //UTF-8 BOM
        if (bytes.empty()) return L"";                            //空文件，含只有一个 BOM 的
        // 剩下的一律按 UTF-8 认：记事本、VSCode 新建的文件都是这个，纯 ASCII 的 JSON 也照样过
        auto len = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), nullptr, 0);
        if (len <= 0) return L"";
        std::wstring str(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), str.data(), len);
        return str;
    }
}


Setting::Setting() :dataPath{ initDataPath() }, configPath{ initConfigPath() }
{
    // 分两句写而不是 dirty = !loadConfig() || ensureDefaults()：那样 ensureDefaults 会被短路掉
    bool dirty = !loadConfig();
    if (ensureDefaults()) dirty = true;
    // 回落到默认值或补过键就写回去：下次启动不用再兜底一遍，用户也能在文件里看到有哪些项可改
    if (dirty) save();
}

bool Setting::loadConfig()
{
    if (std::filesystem::exists(configPath)) {
        auto content = readConfigText(configPath);
        JsonObject obj{ nullptr };
        // 用 TryParse 而不是 Parse：后者解析失败是抛 winrt::hresult_error，
        // 空文件、写坏的 JSON、根节点不是对象（比如一个数组）都会失败，这里一律当没读到
        if (JsonObject::TryParse(content, obj)) {
            configObj = obj;
            return true;
        }
        MessageBox(nullptr, L"config.json parse error，use default config", L"ScreenCapture", MB_OK | MB_ICONWARNING);
    }
    configObj = JsonObject::Parse(defaultConfig); //字面量，不会失败
    return false;
}

bool Setting::ensureDefaults()
{
    auto def = JsonObject::Parse(defaultConfig);
    bool dirty{ false };
    for (auto&& group : def) {                                   //common / shortcutKey
        auto name = group.Key();
        auto defObj = group.Value().GetObject();
        // 带默认值的重载：这一层缺了、或者被手工改成了字符串之类的非对象，都返回 nullptr
        auto obj = configObj.GetNamedObject(name, nullptr);
        if (!obj) {
            configObj.SetNamedValue(name, defObj);
            dirty = true;
            continue;
        }
        for (auto&& item : defObj) {
            if (obj.HasKey(item.Key())) continue;                 //用户自己的值一概不动
            obj.SetNamedValue(item.Key(), item.Value());
            dirty = true;
        }
    }
    return dirty;
}

Setting::~Setting()
{
    auto lingApp = Ling::App::get();
    lingApp->unRegHotKey(capShortcutMsgId);
    lingApp->unRegHotKey(longShortcutMsgId);
    lingApp->unRegHotKey(videoShortcutMsgId);
}

void Setting::init()
{
    auto ptr = new Setting();
    setting.reset(ptr);
}

void Setting::dispose()
{
    setting.reset();
}

Setting* Setting::get()
{
    return setting.get();
}

std::filesystem::path Setting::getDataPath()
{
    return dataPath; //复制一份路径对象，不允许就地修改
}

const JsonObject Setting::getConfigObj()
{
    return configObj;
}

void Setting::setShortcutKey(const std::wstring& type, const std::vector<std::wstring>& keys)
{
    std::wstring str;
    for (const auto& key : keys) {
        if (!str.empty()) str += L"+";
        str += key;
    }
    auto shortcutKey = configObj.GetNamedObject(L"shortcutKey");
    shortcutKey.SetNamedValue(type, JsonValue::CreateStringValue(str));
    auto msgId = shortcutMsgId(type);
    if (msgId != 0) {
        auto app = Ling::App::get();
        app->unRegHotKey(msgId);
        if (!str.empty()) app->regHotKey(str, msgId);
    }
    save();
}

std::wstring Setting::getShortcutKey(const std::wstring& type)
{
    // 一路用带默认值的重载：启动时 ensureDefaults 已经补齐过，这里只是别让运行期
    // 意外（配置被外部改动、问了个没配过的 type）变成一次崩溃
    auto obj = configObj.GetNamedObject(L"shortcutKey", nullptr);
    if (!obj) return L"";
    return std::wstring{ obj.GetNamedString(type, L"") };
}

void Setting::setAutoStart(bool autoStart)
{
    std::wstring runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (autoStart) {
        wchar_t buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto curPath = std::filesystem::path(buffer);
        std::wstring commandLine = std::format(L"\"{}\" --auto-start", curPath.wstring());
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegSetValueEx(hKey, L"ScreenCapture", 0, REG_SZ, (const BYTE*)commandLine.data(), (commandLine.size() + 1) * sizeof(wchar_t));
            RegCloseKey(hKey);
        }
    }
    else {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegDeleteValue(hKey, L"ScreenCapture");
            RegCloseKey(hKey);
        }
    }
    auto common = configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"autoStart", JsonValue::CreateBooleanValue(autoStart));
    save();
}

bool Setting::getAutoStart()
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    return common && common.GetNamedBoolean(L"autoStart", false);
}

std::filesystem::path Setting::initDataPath()
{
    PWSTR pathTmp;
    auto hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
    if (FAILED(hr)) {
        _ASSERT_EXPR(FALSE, L"get roaming path，error");
        return L"";
    }
    auto dataPath = std::filesystem::path{ pathTmp };
    CoTaskMemFree(pathTmp);
    dataPath.append("ScreenCapture");
    if (!std::filesystem::exists(dataPath)) {
        if (!std::filesystem::create_directories(dataPath)) {
            _ASSERT_EXPR(FALSE, L"create data path，error");
        }
    }
    return dataPath;
}

std::filesystem::path Setting::initConfigPath()
{
    // 与插件的查找顺序一致（见 Util.cpp 里的 findImageReader）：先看 exe 同目录。
    // 只有那份文件本来就存在时才认它 —— 不存在就不要在程序目录里新建，
    // 装在 Program Files 下时那儿通常没有写权限，况且默认位置该是 appdata
    wchar_t buffer[MAX_PATH]{};
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    auto path = std::filesystem::path{ buffer }.parent_path().append(L"config.json");
    if (std::filesystem::exists(path)) return path;
    auto fallback = this->dataPath; //复制一份路径对象，append 会就地改
    return fallback.append(L"config.json");
}

void Setting::save()
{
    std::wstring str{ configObj.Stringify() };
    Ling::Util::saveFile(configPath.wstring(), str);
}

std::wstring Setting::getLang()
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    if (!common) return L"zh-CN";
    return std::wstring{ common.GetNamedString(L"language", L"zh-CN") };
}

void Setting::setLang(const std::wstring& langCode)
{
    auto common = setting->configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"language", JsonValue::CreateStringValue(langCode));
    setting->save();
	Lang::get()->initLang(langCode);
}

JsonObject Setting::getToolObj(const std::wstring& tool)
{
    // 用带默认值的重载：这两层在旧配置文件里都不存在，直接 GetNamedObject 会抛异常，
    // 值被手工改成非对象时它也一样返回默认值，不会炸
    auto root = configObj.GetNamedObject(L"toolPin", nullptr);
    if (!root) {
        root = JsonObject();
        configObj.SetNamedValue(L"toolPin", root);
    }
    auto obj = root.GetNamedObject(tool, nullptr);
    if (!obj) {
        obj = JsonObject();
        root.SetNamedValue(tool, obj);
    }
    return obj;
}

bool Setting::getToolFlag(const std::wstring& tool, const std::wstring& key, bool def)
{
    return getToolObj(tool).GetNamedBoolean(key, def);
}

void Setting::setToolFlag(const std::wstring& tool, const std::wstring& key, bool val)
{
    getToolObj(tool).SetNamedValue(key, JsonValue::CreateBooleanValue(val));
    save();
}

float Setting::getToolNum(const std::wstring& tool, const std::wstring& key, float def)
{
    return static_cast<float>(getToolObj(tool).GetNamedNumber(key, def));
}

void Setting::setToolNum(const std::wstring& tool, const std::wstring& key, float val)
{
    getToolObj(tool).SetNamedValue(key, JsonValue::CreateNumberValue(val));
    save();
}

long long Setting::getUpdateCheckDay()
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    if (!common) return 0;
    return static_cast<long long>(common.GetNamedNumber(L"updateCheckDay", 0));
}

void Setting::setUpdateCheckDay(long long day)
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    if (!common) return;
    // 这项不写进 defaultConfig：它是程序自己的记账，不是给用户改的配置
    common.SetNamedValue(L"updateCheckDay", JsonValue::CreateNumberValue(static_cast<double>(day)));
    save();
}

void Setting::initShortcutKeys()
{
    auto lingApp = Ling::App::get();
    auto registerShortcut = [lingApp](const std::wstring& type, UINT msgId) {
        auto shortcut = Setting::get()->getShortcutKey(type);
        if (!shortcut.empty()) lingApp->regHotKey(shortcut, msgId);
    };
    registerShortcut(L"cap", capShortcutMsgId);
    registerShortcut(L"long", longShortcutMsgId);
    registerShortcut(L"video", videoShortcutMsgId);

    lingApp->onHotKey.add([this](UINT msg) {
        if (msg == capShortcutMsgId) {
            WinCap::init();
        }
        else if (msg == longShortcutMsgId) {
            WinCap::init(L"long");
        }
        else if (msg == videoShortcutMsgId) {
            WinCap::init(L"video");
        }
    });
    lingApp->onSecondInstance.add([this]() {
        WinCap::init();
    });
}
