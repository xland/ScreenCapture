#include "pch.h"
#include "../Lang.h"
#include "WinSetting.h"
#include "WinSettingShortcut.h"

WinSettingShortcut::WinSettingShortcut(Ling::WinBase* parent):Ling::Node(parent)
{
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(Lang::get(L"shortcut.cap"));
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setText(L"\ue687");
        btn->setFontFamily(L"icon");
        btn->setHeightPercent(100.f);
        btn->setFontSize(18.f);
        btn->setWidth(60.f);

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(Lang::get(L"shortcut.long"));
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setText(L"简体中文");
        btn->setHeight(28.f);
        btn->setWidth(120.f);
        btn->setBg(0xFFFFFFFF);
        btn->setBorderRadius(5.f);
        btn->setBorder(1.f, 0xE0E0E0FF);

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

}

WinSettingShortcut::~WinSettingShortcut()
{

}
