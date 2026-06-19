#include "pch.h"
#include "Setting.h"
#include "Util.h"

std::unique_ptr<Setting> setting;

Setting::Setting() :dataPath{initDataPath()}
{
    initSettings();
}

Setting::~Setting()
{
}

void Setting::init()
{
	setting = std::make_unique<Setting>();
}

std::filesystem::path Setting::getDataPath()
{
    return setting->dataPath;
}

const std::vector<UINT>& Setting::getCapKeys()
{
    return setting->capKeys;
}

const std::vector<UINT>& Setting::getLongKeys()
{
    return setting->longKeys;
}

const std::vector<UINT>& Setting::getVideoKeys()
{
    return setting->videoKeys;
}


std::wstring Setting::getCapKeysStr()
{
    std::wstring result;
    for (size_t i = 0; i < setting->capKeys.size(); i++)
    {
        result += L"+"+ Util::keyToStr(setting->capKeys[i]);
    }
    return result.substr(1);
}

std::wstring Setting::getLongKeysStr()
{
    std::wstring result;
    for (size_t i = 0; i < setting->longKeys.size(); i++)
    {
        result += L"+" + Util::keyToStr(setting->longKeys[i]);
    }
    return result.substr(1);
}

std::wstring Setting::getVideoKeysStr()
{
    std::wstring result;
    for (size_t i = 0; i < setting->videoKeys.size(); i++)
    {
        result += L"+" + Util::keyToStr(setting->videoKeys[i]);
    }
    return result.substr(1);
}

void Setting::setKeys(const int& type, const std::vector<UINT>& keys)
{
    if (type == 0) {
        setting->capKeys = keys;
    }
    else if (type == 1) {
        setting->longKeys = keys;
    }
    else if (type == 2) {
        setting->videoKeys = keys;
    }
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
        JsonObject jsonObj = JsonObject::Parse(content.data());
        initShortcutKeys(jsonObj.GetNamedObject(L"shortcutKey"));
    }
    else {
        capKeys = { VK_CONTROL,VK_MENU,'A' }; //17 18 65
        longKeys = { VK_CONTROL,VK_MENU,'L' }; //17 18 76
        videoKeys = { VK_CONTROL,VK_MENU,'V' }; //17 18 86
    }
}

void Setting::initShortcutKeys(const JsonObject& jsonObj)
{
    std::wstring str{ jsonObj.GetNamedString(L"cap") };
    auto arr = Util::splitStr(str, L'+');
    for (size_t i = 0; i < arr.size(); i++)
    {
        capKeys.push_back(Util::strToKey(arr[i]));
    }

    str = jsonObj.GetNamedString(L"long");
    arr = Util::splitStr(str, L'+');
    for (size_t i = 0; i < arr.size(); i++)
    {
        longKeys.push_back(Util::strToKey(arr[i]));
    }

    str =  jsonObj.GetNamedString(L"video");
    arr = Util::splitStr(str, L'+');
    for (size_t i = 0; i < arr.size(); i++)
    {
        videoKeys.push_back(Util::strToKey(arr[i]));
    }
}

void Setting::save()
{
    JsonObject obj;
    obj.SetNamedValue(L"cap", JsonValue::CreateStringValue(Setting::getCapKeysStr()));
    obj.SetNamedValue(L"long", JsonValue::CreateStringValue(Setting::getLongKeysStr()));
    obj.SetNamedValue(L"video", JsonValue::CreateStringValue(Setting::getVideoKeysStr()));
    JsonObject config;
    config.SetNamedValue(L"shortcutKey", obj);
    std::wstring str{ config.Stringify() };
    auto dataPath = this->dataPath;
    auto pathStr = dataPath.append("config.json").wstring();
    Util::saveFile(pathStr, str);
}


