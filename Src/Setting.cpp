#include "pch.h"
#include <include/Ling.h>
#include "Setting.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "App.h"

namespace {
    std::unique_ptr<Setting> setting;
    constexpr int capShortcutMsgId{ 100 };
}


Setting::Setting() :dataPath{initDataPath()}
{
    auto dataPath = this->dataPath; //复制一份路径对象
    auto path = dataPath.append("config.json");
    if (std::filesystem::exists(path)) {
        auto pathStr = path.wstring();
        std::wstring content = Ling::Util::readFile(pathStr);
        configObj = JsonObject::Parse(content.data());
    }
    else {
        configObj = JsonObject::Parse(LR"""({"common":{"autoStart":false,"language":"zh-CN"},"shortcutKey":{"cap":"Ctrl+Alt+A"}})""");
    }
}

Setting::~Setting()
{
    //auto lingApp = Ling::App::get();
    //lingApp->unRegHotKey(capShortcutMsgId);
}

void Setting::init()
{
    auto ptr = new Setting();
    setting.reset(ptr);
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
    for (size_t i = 0; i < keys.size(); i++)
    {
        str += L"+" + keys[i];
    }
    str.erase(0,1);
    auto shortcutKey = configObj.GetNamedObject(L"shortcutKey");
    shortcutKey.SetNamedValue(type, JsonValue::CreateStringValue(str));
    auto app = Ling::App::get();
    app->unRegHotKey(capShortcutMsgId);
    app->regHotKey(str, capShortcutMsgId);
    save();
}

std::wstring Setting::getShortcutKey(const std::wstring& type)
{
    auto obj = configObj.GetNamedObject(L"shortcutKey");
    return std::wstring{ obj.GetNamedString(type) };
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
    auto common = configObj.GetNamedObject(L"common");
    return common.GetNamedBoolean(L"autoStart");
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

void Setting::save()
{
    std::wstring str{ configObj.Stringify() };
    auto dataPath = this->dataPath;
    auto pathStr = dataPath.append("config.json").wstring();
    Ling::Util::saveFile(pathStr, str);
}

std::wstring Setting::getLang()
{
    auto common = configObj.GetNamedObject(L"common");
    auto lang = common.GetNamedString(L"language");
    return std::wstring{ lang };
}

void Setting::setLang(const std::wstring& langCode)
{
    auto common = setting->configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"language", JsonValue::CreateStringValue(langCode));
    setting->save();
	Lang::get()->initLang(langCode);
}

void Setting::initShortcutKeys()
{
    auto lingApp = Ling::App::get();
    auto configShortcut = Setting::get()->getConfigObj().GetNamedObject(L"shortcutKey");
    std::wstring capStr{ configShortcut.GetNamedString(L"cap") };
    lingApp->regHotKey(capStr, capShortcutMsgId);

    lingApp->onHotKey.add([this](UINT msg) {
        if (msg == capShortcutMsgId) {
            WinCap::init();
        }
    });
    lingApp->onSecondInstance.add([this]() {
        WinCap::init();
    });
}
