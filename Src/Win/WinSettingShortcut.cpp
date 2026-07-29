#include "pch.h"
#include "../Lang.h"
#include "../Setting.h"
#include "WinSetting.h"
#include "WinSettingShortcut.h"

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
}

WinSettingShortcut::~WinSettingShortcut()
{
    *alive = false;
    win->onMouseDown.remove(onMouseDownToken);
}

void WinSettingShortcut::onBtnClick(Ling::Button* btn)
{
    btn->setText(Lang::get(L"shortcut.pressKey"));
    resetCurKey();
    curKey = btn->id;
    auto guard = alive;
    onMouseDownToken = win->onMouseDown.add([this, guard](POINT pos, bool isRight) {
        if (!*guard) return;
        win->onMouseDown.remove(onMouseDownToken);
        resetCurKey();
    });
}

void WinSettingShortcut::resetCurKey()
{
    if (!curKey.empty()) {
        for (auto& btn : btns)
        {
            if (btn->id == curKey) {
                btn->setText(Setting::get()->getShortcutKey(curKey));
                break;
            }
        }
    }
}
