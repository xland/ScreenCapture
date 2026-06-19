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
        result += L"+"+setting->keyToStr(setting->capKeys[i]);
    }
    return result.substr(1);
}

std::wstring Setting::getLongKeysStr()
{
    std::wstring result;
    for (size_t i = 0; i < setting->longKeys.size(); i++)
    {
        result += L"+" + setting->keyToStr(setting->longKeys[i]);
    }
    return result.substr(1);
}

std::wstring Setting::getVideoKeysStr()
{
    std::wstring result;
    for (size_t i = 0; i < setting->videoKeys.size(); i++)
    {
        result += L"+" + setting->keyToStr(setting->videoKeys[i]);
    }
    return result.substr(1);
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
    dataPath.append("config.json");
    if (std::filesystem::exists(dataPath)) {
        std::ifstream file(dataPath);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto dataStr = Util::convertToWStr(content.data());
        JsonObject jsonObj = JsonObject::Parse(dataStr.data());
        auto str = jsonObj.GetNamedString(L"capKey");
    }
    else {
        capKeys = { VK_CONTROL,VK_MENU,'A' };
        longKeys = { VK_CONTROL,VK_MENU,'L' };
        videoKeys = { VK_CONTROL,VK_MENU,'V' };
    }
}

std::wstring Setting::keyToStr(UINT vkCode)
{
    switch (vkCode) {
        // --- 修饰键 ---
    case VK_CONTROL: return L"Ctrl";
    case VK_MENU:    return L"Alt";
    case VK_SHIFT:   return L"Shift";
    case VK_LWIN:    return L"LWin";
    case VK_RWIN:    return L"RWin";
    case VK_CAPITAL: return L"CapsLock";

        // --- 功能键 (F1 - F12) ---
    case VK_F1: return L"F1";
    case VK_F2: return L"F2";
    case VK_F3: return L"F3";
    case VK_F4: return L"F4";
    case VK_F5: return L"F5";
    case VK_F6: return L"F6";
    case VK_F7: return L"F7";
    case VK_F8: return L"F8";
    case VK_F9: return L"F9";
    case VK_F10: return L"F10";
    case VK_F11: return L"F11";
    case VK_F12: return L"F12";

        // --- 方向键 ---
    case VK_UP: return L"Up";
    case VK_DOWN: return L"Down";
    case VK_LEFT: return L"Left";
    case VK_RIGHT: return L"Right";

        // --- 控制与编辑键 ---
    case VK_RETURN:  return L"Enter";
    case VK_ESCAPE:  return L"Esc";
    case VK_TAB:     return L"Tab";
    case VK_SPACE:   return L"Space";
    case VK_BACK:    return L"Backspace";
    case VK_DELETE:  return L"Delete";
    case VK_INSERT:  return L"Insert";
    case VK_HOME:    return L"Home";
    case VK_END:     return L"End";
    case VK_PRIOR:   return L"PageUp";
    case VK_NEXT:    return L"PageDown";
    case VK_SNAPSHOT:return L"PrintScreen";
    case VK_SCROLL:  return L"ScrollLock";
    case VK_PAUSE:   return L"Pause";

        // --- 小键盘 ---
    case VK_NUMLOCK: return L"NumLock";
    case VK_MULTIPLY: return L"*";
    case VK_ADD:      return L"+";
    case VK_SUBTRACT: return L"-";
    case VK_DIVIDE:   return L"/";
    case VK_DECIMAL:  return L".";

    default:
        // --- 字母键 (A-Z 对应 ASCII 码 65-90) ---
        if (vkCode >= 'A' && vkCode <= 'Z') {
            return std::wstring(1, static_cast<wchar_t>(vkCode));
        }
        // --- 主键盘数字键 (0-9 对应 ASCII 码 48-57) ---
        if (vkCode >= '0' && vkCode <= '9') {
            return std::wstring(1, static_cast<wchar_t>(vkCode));
        }
        // --- 小键盘数字键 (0-9) ---
        if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9) {
            return std::to_wstring(vkCode - VK_NUMPAD0);
        }

        // --- 未识别的键，返回带前缀的数字 ---
        return std::to_wstring(vkCode);
    }
}

UINT Setting::strToKey(const std::wstring& keyName)
{
    if (keyName.empty()) return 0;

    std::wstring lowerName = keyName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);
    if (keyName.length() == 1) {
        wchar_t c = keyName[0];
        if (c >= 'A' && c <= 'Z') return static_cast<UINT>(c);
        if (c >= 'a' && c <= 'z') return static_cast<UINT>(std::towupper(c));
        if (c >= '0' && c <= '9') return static_cast<UINT>(c);
    }
    // --- 修饰键 ---
    if (lowerName == L"ctrl") return VK_CONTROL;
    if (lowerName == L"alt") return VK_MENU;
    if (lowerName == L"shift") return VK_SHIFT;
    if (lowerName == L"lwin") return VK_LWIN;
    if (lowerName == L"rwin") return VK_RWIN;
    if (lowerName == L"capslock") return VK_CAPITAL;

    // --- 功能键 ---
    if (lowerName == L"f1") return VK_F1;
    if (lowerName == L"f2") return VK_F2;
    if (lowerName == L"f3") return VK_F3;
    if (lowerName == L"f4") return VK_F4;
    if (lowerName == L"f5") return VK_F5;
    if (lowerName == L"f6") return VK_F6;
    if (lowerName == L"f7") return VK_F7;
    if (lowerName == L"f8") return VK_F8;
    if (lowerName == L"f9") return VK_F9;
    if (lowerName == L"f10") return VK_F10;
    if (lowerName == L"f11") return VK_F11;
    if (lowerName == L"f12") return VK_F12;

    // --- 方向键 ---
    if (lowerName == L"up") return VK_UP;
    if (lowerName == L"down") return VK_DOWN;
    if (lowerName == L"left") return VK_LEFT;
    if (lowerName == L"right") return VK_RIGHT;

    // --- 控制与编辑键 ---
    if (lowerName == L"enter") return VK_RETURN;
    if (lowerName == L"esc") return VK_ESCAPE;
    if (lowerName == L"space") return VK_SPACE;
    if (lowerName == L"tab") return VK_TAB;
    if (lowerName == L"backspace") return VK_BACK;
    if (lowerName == L"delete") return VK_DELETE;
    if (lowerName == L"insert") return VK_INSERT;
    if (lowerName == L"home") return VK_HOME;
    if (lowerName == L"end") return VK_END;
    if (lowerName == L"pageup") return VK_PRIOR;
    if (lowerName == L"pagedown") return VK_NEXT;
    if (lowerName == L"printscreen") return VK_SNAPSHOT;
    if (lowerName == L"scrolllock") return VK_SCROLL;
    if (lowerName == L"pause") return VK_PAUSE;

    // --- 小键盘符号 ---
    if (lowerName == L"numlock") return VK_NUMLOCK;
    if (lowerName == L"num *") return VK_MULTIPLY;
    if (lowerName == L"num +") return VK_ADD;
    if (lowerName == L"num -") return VK_SUBTRACT;
    if (lowerName == L"num /") return VK_DIVIDE;
    if (lowerName == L"num .") return VK_DECIMAL;

    return 0;
}
