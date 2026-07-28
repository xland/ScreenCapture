#include "pch.h"
#include <include/Ling.h>
#include "Setting.h"
#include "App.h"

std::unique_ptr<Setting> setting;

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
        configObj = JsonObject::Parse(LR"""({"common":{"autoStart":false,"language":"zh-CN"},"shortcutKey":{"cap":"Ctrl+Alt+A","long":"Ctrl+Alt+L","video":"Ctrl+Alt+V"}})""");
    }
}

Setting::~Setting()
{
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

void Setting::setKeys(const int& type, const std::vector<std::wstring>& keys)
{
    std::wstring str;
    for (size_t i = 0; i < keys.size(); i++)
    {
        str += L"+" + keys[i];
    }
    str.erase(0,1);

    auto shortcutKey = configObj.GetNamedObject(L"shortcutKey");
    if (type == 0) {
        shortcutKey.SetNamedValue(L"cap",JsonValue::CreateStringValue(str));
    }
    else if (type == 1) {
        shortcutKey.SetNamedValue(L"long", JsonValue::CreateStringValue(str));
    }
    else if (type == 2) {
        shortcutKey.SetNamedValue(L"video", JsonValue::CreateStringValue(str));
    }
    save();
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

std::wstring Setting::getLanguage()
{
    auto common = configObj.GetNamedObject(L"common");
    auto lang = common.GetNamedString(L"language");
    return std::wstring{ lang };
}

void Setting::setLanguage(const std::wstring& lang)
{
    auto common = setting->configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"language", JsonValue::CreateStringValue(lang));
    setting->save();
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


