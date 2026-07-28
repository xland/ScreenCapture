#include "pch.h"
#include "../Lang.h"
#include "../Setting.h"
#include "WinSetting.h"
#include "WinSettingCommon.h"

WinSettingCommon::WinSettingCommon(Ling::WinBase* parent):Ling::Node(parent)
{
    
    initAutoStartCtrls();
    initLangCtrls();
    win->onMouseDown.add([this](POINT, bool) {
        if (suspendMouseDownFlag) {
            suspendMouseDownFlag = false;
            return;
        }
        if (selectBox) {
            win->body->removeChild(selectBox);
        }
    });
}

WinSettingCommon::~WinSettingCommon()
{

}

void WinSettingCommon::initAutoStartCtrls()
{
    auto box = makeChild<Ling::Node>();
    box->setHeight(39.f);
    box->setFlexDirection(Ling::FlexDirection::Row);
    box->setAlignItems(Ling::Align::Center);

    auto label = box->makeChild<Ling::Label>();
    label->setText(Lang::get(L"setting.autoStart"));
    label->setHeightPercent(100.f);
    label->setJustifyContent(Ling::Justify::Center);
    label->setFlexGrow(1.f);

    auto btn = box->makeChild<Ling::Button>();
    btn->setText(L"\ue687");
    btn->setFontFamily(L"icon");
    btn->setHeightPercent(100.f);
    btn->setFontSize(18.f);
    btn->setWidth(60.f);
    setAutoStartBtn(btn);

    btn->onClick.add([this](Ling::Button* btn) {
        auto setting = Setting::get();
        auto isAutoStart = setting->getAutoStart();
        setting->setAutoStart(!isAutoStart);
        setAutoStartBtn(btn);
    });

    auto border = makeChild<Ling::Node>();
    border->setHeight(1.f);
    border->setBg(0xE0E0E0FF);
}

void WinSettingCommon::initLangCtrls()
{
    auto box = makeChild<Ling::Node>();
    box->setHeight(39.f);
    box->setFlexDirection(Ling::FlexDirection::Row);
    box->setAlignItems(Ling::Align::Center);

    auto label = box->makeChild<Ling::Label>();
    label->setText(Lang::get(L"setting.language"));
    label->setHeightPercent(100.f);
    label->setJustifyContent(Ling::Justify::Center);
    label->setFlexGrow(1.f);

    auto btn = box->makeChild<Ling::Button>();
    btn->setText(L"简体中文");
    btn->setHeight(28.f);
    btn->setWidth(132.f);
    btn->setBorder(1.f, 0xE0E0E0FF);
    btn->setHoverBg(0XFFFFFFFF);
    btn->onClick.add([this](Ling::Button* btn) {this->showSelectBox(btn);});
    //btn->setHoverBorderColor(0x597ef7ff);

    auto border = makeChild<Ling::Node>();
    border->setHeight(1.f);
    border->setBg(0xE0E0E0FF);
}

void WinSettingCommon::setAutoStartBtn(Ling::Button* btn)
{
    auto setting = Setting::get();
    auto isAutoStart = setting->getAutoStart();
    if (isAutoStart) {
        btn->setText(L"\ue688");
        btn->setColor(0x597ef7ff);
        btn->setHoverColor(0x597ef7ff);
    }
    else {
        btn->setText(L"\ue687");
        btn->setColor(0x666666FF);
        btn->setHoverColor(0x666666FF);
    }
}

void WinSettingCommon::showSelectBox(Ling::Button* btn)
{
    suspendMouseDownFlag = true;
    if (selectBox) {
        win->body->removeChild(selectBox);
    }
    auto langs = Lang::get()->getSupportedLang();
    auto itemH{ 30.f };
    auto totalH = std::min(320.f, itemH * (langs.size()+1));

    selectBox = win->body->makeChild<Ling::ScrollerBox>();
    selectBox->setSize(btn->w/win->dpi, totalH);
    selectBox->setPositionType(Ling::Position::Absolute);
    selectBox->setPosition(Ling::Edge::Left, btn->x/win->dpi);
    selectBox->setPosition(Ling::Edge::Top, btn->y/win->dpi);
    selectBox->setBg(0xFFFFFFFF);
    selectBox->setBorder(1.f, 0xE0E0E0FF);
    for (auto& pair:langs)
    {
        auto btn = selectBox->makeChild<Ling::Button>();
        btn->setText(pair.first);
        btn->setHeight(itemH);
        btn->setWidthPercent(100.f);
        btn->setHoverBg(0Xf2f2f2FF);
        btn->setHoverColor(0X000000FF);
        btn->onClick.add([this](Ling::Button* btn) {;});
    }
    auto lastItem = selectBox->makeChild<Ling::Button>();
    lastItem->setText(Lang::get(L"setting.getMoreLang"));
    lastItem->setHeight(itemH);
    lastItem->setWidthPercent(100.f);
    lastItem->setHoverBg(0Xf2f2f2FF);
    lastItem->setHoverColor(0X000000FF);
    lastItem->onClick.add([this](Ling::Button* btn) {
        std::wstring downloadUrl{ L"https://github.com/xland/ScreenCapture/Lang" };
        ShellExecute(win->hwnd, L"open", downloadUrl.data(), nullptr, nullptr, SW_SHOWNORMAL);
        });
}
