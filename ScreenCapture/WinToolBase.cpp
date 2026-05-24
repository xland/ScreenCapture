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

void WinToolBase::addBtns(const std::vector<std::pair<std::wstring, std::wstring>>& btns)
{
    auto fontSize{ 14.f * dpi };
    for (const auto& pair : btns)
    {
        btnIcons.push_back({ pair.first ,getIconLayout(pair.second, fontSize, h, h) });
    }
}
