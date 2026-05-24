#include "pch.h"
#include "WinToolBase.h"

WinToolBase::WinToolBase(const int& x, const int& y, const int& w, const int& h) :
    WinBase(x, y, w, h)
{}


WinToolBase::~WinToolBase()
{

}

void WinToolBase::initBrush()
{
    render->CreateSolidColorBrush(D2D1::ColorF(0xffffff), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0xe6f4ff), brushSelect.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x333333), brushIcon.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBlue.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x888888), brushSpliter.GetAddressOf());
}

void WinToolBase::paintIcon(const float& posX, IDWriteTextLayout* icon, bool isHover, bool isSelected)
{
    D2D1_POINT_2F origin = { posX, marginTop };
    if (isSelected) {
        float paddingTopBottom{ 4.f * dpi }, paddingLeftRight{ 2.6f * dpi };
        D2D1_ROUNDED_RECT rr = { { posX + paddingLeftRight, paddingTopBottom+marginTop, posX + btnSize - paddingLeftRight, h - paddingTopBottom }, 8, 8 };
        render->FillRoundedRectangle(rr, brushSelect.Get());
        render->DrawTextLayout(origin, icon, brushBlue.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    else {
        auto brush = isHover ? brushBlue.Get() : brushIcon.Get();
        render->DrawTextLayout(origin, icon, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void WinToolBase::addBtns(const std::vector<std::pair<std::wstring, std::wstring>>& btns)
{
    auto fontSize{ 14.f * dpi };
    for (const auto& pair : btns)
    {
        btnIcons.push_back({ pair.first ,getIconLayout(pair.second, fontSize, btnSize, btnSize) });
    }
}

IDWriteTextLayout* WinToolBase::getBtnIconLayout(const std::wstring& name)
{
    //从btnIcons获取图标布局
    for (const auto& pair : btnIcons)
    {
        if (pair.first == name)
        {
            return pair.second.Get();
        }
    }
}
