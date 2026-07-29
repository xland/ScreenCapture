#include "pch.h"
#include "../Lang.h"
#include "../Setting.h"
#include "WinSetting.h"
#include "WinSettingShortcut.h"

namespace {
    bool isShortcutModifierKey(const std::wstring& key)
    {
        return key == L"Ctrl" || key == L"Alt" || key == L"Shift" || key == L"Win" || key == L"LWin" || key == L"RWin";
    }

    bool isForbiddenShortcutCommonKey(const std::wstring& key)
    {
        return key == L"Enter" || key == L"Backspace" || key == L"Delete";
    }

    bool isForbiddenCtrlCommonShortcutKey(const std::wstring& key)
    {
        return key == L"Z" || key == L"Y" || key == L"S" || key == L"C" || key == L"X" || key == L"A";
    }
    bool isValidShortcutKeys(const std::vector<std::wstring>& keys)
    {
        bool hasModifier = false;
        bool hasNormalKey = false;
        bool hasCtrl = false;
        bool hasOtherModifier = false;
        bool hasForbiddenCtrlCommonKey = false;
        for (const auto& key : keys) {
            if (isShortcutModifierKey(key)) {
                hasModifier = true;
                if (key == L"Ctrl") {
                    hasCtrl = true;
                }
                else {
                    hasOtherModifier = true;
                }
                continue;
            }
            hasNormalKey = true;
            if (isForbiddenShortcutCommonKey(key)) {
                return false;
            }
            if (isForbiddenCtrlCommonShortcutKey(key)) {
                hasForbiddenCtrlCommonKey = true;
            }
        }
        return hasModifier && hasNormalKey && !(hasCtrl && !hasOtherModifier && hasForbiddenCtrlCommonKey);
    }

    // 修饰键固定排序：Ctrl > Alt > Shift > Win > LWin > RWin > 普通键
    int modifierRank(const std::wstring& key)
    {
        if (key == L"Ctrl")  return 0;
        if (key == L"Alt")   return 1;
        if (key == L"Shift") return 2;
        if (key == L"Win")   return 3;
        if (key == L"LWin")  return 4;
        if (key == L"RWin")  return 5;
        return 100; // 普通键都排到修饰键之后
    }

    void normalizeShortcutKeys(std::vector<std::wstring>& keys)
    {
        std::stable_sort(keys.begin(), keys.end(),
            [](const std::wstring& a, const std::wstring& b) {
                return modifierRank(a) < modifierRank(b);
            });
    }
}



WinSettingShortcut::WinSettingShortcut(Ling::WinBase* parent):Ling::Node(parent)
{
    std::vector<std::wstring> keys = { L"cap",L"long",L"video" };
    for (auto& key:keys)
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(Lang::get(L"shortcut."+key));
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setId(key);
        btn->setText(Setting::get()->getShortcutKey(key));
        btn->setHeight(28.f);
        btn->setWidth(120.f);
        btn->setBg(0xFFFFFFFF);
        btn->setHoverBg(0xFFFFFFFF);
        btn->setBorder(1.f, 0xE0E0E0FF);
        btn->onClick.add([this](Ling::Button* btn) {this->onBtnClick(btn);});
        btns.push_back(btn);

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

    auto guard = alive;
    win->onKeyDown.add([this, guard](UINT key) {
        if (!*guard || curKey.empty()) return;
        this->onKeyDown(key);
    });
    win->onKeyUp.add([this, guard](UINT key) {
        if (!*guard || curKey.empty()) return;
        this->onKeyUp(key);
    });
    onMouseDownToken = win->onMouseDown.add([this, guard](POINT pos, bool isRight) {
        if (!*guard || curKey.empty()) return;
        for (auto btn: btns)
        {
            if(btn->isPosIn(pos)) return;
        }
        endCapture();
    });
}

WinSettingShortcut::~WinSettingShortcut()
{
    *alive = false;
    win->onMouseDown.remove(onMouseDownToken);
}

void WinSettingShortcut::onBtnClick(Ling::Button* btn)
{
    // 再次点击当前正在捕获的按钮 → 取消
    if (curKey == btn->id) {
        endCapture();
        return;
    }
    // 已经在捕获别的按钮 → 先复位旧的
    if (!curKey.empty()) {
        endCapture();
    }
    beginCapture(btn);
}

void WinSettingShortcut::beginCapture(Ling::Button* btn)
{
    curKey = btn->id;
    tempKeys.clear();
    btn->setText(Lang::get(L"shortcut.pressKey"));
}

void WinSettingShortcut::endCapture()
{
    if (curKey.empty()) return;
    for (auto& btn : btns)
    {
        if (btn->id == curKey) {
            btn->setText(Setting::get()->getShortcutKey(curKey));
            break;
        }
    }
    curKey.clear();
    tempKeys.clear();
}

void WinSettingShortcut::onKeyDown(UINT key)
{
    auto keyStr = keyToStr(key);
    if (keyStr.empty()) return;

    // 当按下的是普通键（非修饰键）时，主动补齐当前按住的修饰键
    // 因为 Alt 等键走 WM_SYSKEYDOWN，Ling 框架可能不会转发到 onKeyDown
    if (!isShortcutModifierKey(keyStr)) {
        auto ensure = [&](int vk, const std::wstring& name) {
            if ((GetAsyncKeyState(vk) & 0x8000) &&
                std::find(tempKeys.begin(), tempKeys.end(), name) == tempKeys.end()) {
                tempKeys.push_back(name);
            }
        };
        ensure(VK_CONTROL, L"Ctrl");
        ensure(VK_MENU,    L"Alt");
        ensure(VK_SHIFT,   L"Shift");
        ensure(VK_LWIN,    L"LWin");
        ensure(VK_RWIN,    L"RWin");
    }

    bool isContains = std::find(tempKeys.begin(), tempKeys.end(), keyStr) != tempKeys.end();
    if (isContains) return;
    tempKeys.push_back(keyStr);
}

void WinSettingShortcut::onKeyUp(UINT key)
{
    if (curKey.empty()) return;
    normalizeShortcutKeys(tempKeys);
    if (isValidShortcutKeys(tempKeys)) {
        Setting::get()->setShortcutKey(curKey, tempKeys);
    }
    endCapture();
}

std::wstring WinSettingShortcut::keyToStr(UINT vkCode)
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
            return L"Num" + std::to_wstring(vkCode - VK_NUMPAD0);
        }
        if (vkCode == VK_OEM_3 || vkCode == VK_OEM_1 || vkCode == VK_OEM_4 ||
            vkCode == VK_OEM_6 || vkCode == VK_OEM_7 || vkCode == VK_OEM_5 ||
            vkCode == VK_OEM_2 || vkCode == VK_OEM_COMMA || vkCode == VK_OEM_PERIOD ||
            vkCode == VK_OEM_MINUS || vkCode == VK_OEM_PLUS)
        {
            wchar_t ch = 0;
            // MAPVK_VK_TO_CHAR (2) 会将虚拟键码转换为不带 Shift 状态的基础字符
            UINT scanCode = MapVirtualKeyW(vkCode, MAPVK_VK_TO_VSC);
            int result = MapVirtualKeyW(vkCode, MAPVK_VK_TO_CHAR);

            // 结果的低 16 位是字符，如果最高位(0x80000000)被置位，说明是死键(Dead Key)
            if (result != 0 && !(result & 0x80000000)) {
                ch = static_cast<wchar_t>(result & 0xFFFF);
                return std::wstring(1, ch);
            }
        }
        return L"";
    }
}
