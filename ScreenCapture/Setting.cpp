#include "pch.h"
#include "Setting.h"
#include "Util.h"
#include "App.h"

std::unique_ptr<Setting> setting;

Setting::Setting() :dataPath{initDataPath()}
{
    if (App::getArg(L"auto-quit") == L"false") {
        initSettings();
    }    
}

Setting::~Setting()
{
}

void Setting::init()
{
	setting = std::make_unique<Setting>();
}

void Setting::dispose()
{
    setting.reset();
}

std::filesystem::path Setting::getDataPath()
{
    return setting->dataPath;
}

const JsonObject& Setting::getConfigObj()
{
    return setting->configObj;
}

void Setting::setKeys(const int& type, const std::vector<std::wstring>& keys)
{
    std::wstring str;
    for (size_t i = 0; i < keys.size(); i++)
    {
        str += L"+" + keys[i];
    }
    str.erase(0,1);
    if (type == 0) {
        setting->configObj.GetNamedObject(L"shortcutKey").SetNamedValue(L"cap",JsonValue::CreateStringValue(str));
    }
    else if (type == 1) {
        setting->configObj.GetNamedObject(L"shortcutKey").SetNamedValue(L"long", JsonValue::CreateStringValue(str));
    }
    else if (type == 2) {
        setting->configObj.GetNamedObject(L"shortcutKey").SetNamedValue(L"video", JsonValue::CreateStringValue(str));
    }
    setting->save();
}

void Setting::setSwitch(bool autoStart, bool showTray)
{
    auto common = setting->configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"autoStart", JsonValue::CreateBooleanValue(autoStart));
    common.SetNamedValue(L"showTray", JsonValue::CreateBooleanValue(showTray));
    setting->save();
}

std::wstring Setting::getLanguage()
{
    try {
        auto common = setting->configObj.GetNamedObject(L"common");
        return std::wstring(common.GetNamedString(L"language"));
    }
    catch (...) {
        return L"zh";
    }
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
        return L"";
    }
    auto dataPath = std::filesystem::path{ pathTmp };
    CoTaskMemFree(pathTmp);
    dataPath.append("ScreenCapture");
    if (!std::filesystem::exists(dataPath)) {
        if (!std::filesystem::create_directories(dataPath)) {
            log(L"data path create error");
        }
    }
    return dataPath;
}

void Setting::initSettings()
{
    auto dataPath = this->dataPath;
    auto path = dataPath.append("config.json");
    if (std::filesystem::exists(path)) {
        auto pathStr = path.wstring();
        std::wstring content = Util::readFile(pathStr);
        configObj = JsonObject::Parse(content.data());
        //initShortcutKeys(jsonObj.GetNamedObject(L"shortcutKey"));
    }
    else {
        configObj = JsonObject::Parse(LR"""({"common":{"autoStart":false,"showTray":true,"language":"zh"},"shortcutKey":{"cap":"Ctrl+Alt+A","long":"Ctrl+Alt+L","video":"Ctrl+Alt+V"}})""");
        //capKeys = { L"Ctrl",L"Alt",L"A"}; //17 18 65
        //longKeys = { L"Ctrl",L"Alt",L"L" }; //17 18 76
        //videoKeys = { L"Ctrl",L"Alt",L"V" }; //17 18 86
    }
}

void Setting::save()
{
    std::wstring str{ configObj.Stringify() };
    auto dataPath = this->dataPath;
    auto pathStr = dataPath.append("config.json").wstring();
    Util::saveFile(pathStr, str);
}


