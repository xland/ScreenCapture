#include "pch.h"
#include "WinToolBase.h"
#include "App.h"

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

void WinToolBase::initTip()
{
    INITCOMMONCONTROLSEX iccex = { sizeof(iccex), ICC_BAR_CLASSES };
    InitCommonControlsEx(&iccex);
    tipHwnd = CreateWindowEx(
        WS_EX_TOPMOST, TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd,
        NULL,
        App::get()->hInstance,
        NULL
    );
    SetWindowPos(tipHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = hwnd;
    ti.hinst = App::get()->hInstance;
    ti.uId = 0;
    ti.lpszText = (LPWSTR)L" ";
    ti.rect = { 0, 0, 0, 0 };
    SendMessage(tipHwnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    SendMessage(tipHwnd, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(0, 0));
}
void WinToolBase::showTipAt(int x, int y)
{
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = hwnd;
    ti.hinst = App::get()->hInstance;
    ti.uId = 0;
    ti.lpszText = (LPWSTR)tipText.c_str();
    //ti.lpszText = (LPWSTR)L"测试";
    // 更新文本
    SendMessage(tipHwnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    // 先用临时位置激活 tooltip，让系统计算尺寸
    SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(x-30.f, y-18.6*dpi));
    SendMessage(tipHwnd, TTM_ACTIVATE, TRUE, 0);
    SendMessage(tipHwnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    // 获取 tooltip 实际尺寸
    RECT rc = { 0 };
    GetWindowRect(tipHwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int centerX = x - width / 2;
    int centerY = y - height;
    SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(centerX, centerY));
}
void WinToolBase::hideTip()
{
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.hwnd = hwnd;
    ti.uId = 0;

    // 关闭跟踪激活
    SendMessage(tipHwnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    // 同时关闭普通激活（双重保险）
    SendMessage(tipHwnd, TTM_ACTIVATE, FALSE, 0);
}