#include "pch.h"
#include "Util.h"
#include "WinToolMain.h"
#include "App.h"
#include "WinCap.h"
#include "CutMask.h"
//#include "WinPin.h"


WinToolMain::WinToolMain()
{
    auto app = App::get();
    auto count = std::count_if(app->toolBtns.begin(), app->toolBtns.end(), [](auto& s) { return s != L"|"; });
    createWindow(count);
}
WinToolMain::~WinToolMain()
{
    //brushBg.Reset();
    //brushIcon.Reset();
    //brushIconHover.Reset();
    //brushSpliter.Reset();
    //render.Reset();
}


IDWriteTextLayout* WinToolMain::getIconLayout(const std::wstring& icon)
{
    static std::unordered_map<std::wstring, ComPtr<IDWriteTextLayout>> btnIcons;
    if (!btnIcons.contains(icon)) {
        auto fontSize{ 14.f * dpi };
        btnIcons.clear();
        btnIcons.insert({ L"rect" ,Util::getIconLayout(L"\ue8e8",fontSize,btnW,h) });
        btnIcons.insert({ L"ellipse" ,Util::getIconLayout(L"\ue6bc",fontSize,btnW,h) });
        btnIcons.insert({ L"arrow" ,Util::getIconLayout(L"\ue603",fontSize,btnW,h) });
        btnIcons.insert({ L"number" ,Util::getIconLayout(L"\ue776",fontSize,btnW,h) });
        btnIcons.insert({ L"line" ,Util::getIconLayout(L"\ue601",fontSize,btnW,h) });
        btnIcons.insert({ L"text" ,Util::getIconLayout(L"\ue6ec",fontSize,btnW,h) });
        btnIcons.insert({ L"mosaic" ,Util::getIconLayout(L"\ue82e",fontSize,btnW,h) });
        btnIcons.insert({ L"eraser" ,Util::getIconLayout(L"\ue6be",fontSize,btnW,h) });
        btnIcons.insert({ L"undo" ,Util::getIconLayout(L"\ued85",fontSize,btnW,h) });
        btnIcons.insert({ L"redo" ,Util::getIconLayout(L"\ued8a",fontSize,btnW,h) });
        btnIcons.insert({ L"pin" ,Util::getIconLayout(L"\ue6a2",fontSize,btnW,h) });
        btnIcons.insert({ L"clipboard" ,Util::getIconLayout(L"\ue650",fontSize,btnW,h) });
        btnIcons.insert({ L"save" ,Util::getIconLayout(L"\ue608",fontSize,btnW,h) });
        btnIcons.insert({ L"close" ,Util::getIconLayout(L"\ue62d",fontSize,btnW,h) });
    }
    return btnIcons[icon].Get();
}

void WinToolMain::onPaint()
{
    auto app = App::get();
    int btnIndex{ 0 };
    auto win = WinCap::get();
    for (auto& btn : app->toolBtns)
    {
        if (btn == L"|") {
            auto lx{ btnW * btnIndex }, lt{ 3.f * btnPadding };
            render->DrawLine({ lx,lt }, { lx,btnH - lt }, brushSpliter.Get(), 0.8f);
        }
        else
        {           
            if (btnIndex == hoverIndex || btn == win->drawState) {
                auto xStart{ btnW * btnIndex };
                D2D1_ROUNDED_RECT rr = { { xStart + btnPadding, btnPadding, xStart + btnW - btnPadding, h - btnPadding }, 6, 6 };
                render->FillRoundedRectangle(rr, brushBg.Get());
                D2D1_POINT_2F origin = { xStart, 0.f };
                render->DrawTextLayout(origin, getIconLayout(btn), brushIconHover.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
            else {
                D2D1_POINT_2F origin = { btnW * btnIndex, 0.f };
                render->DrawTextLayout(origin, getIconLayout(btn), brushIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
            btnIndex += 1;
        }
    }
}

void WinToolMain::onClick()
{
    auto index{ 0 };
    for (auto& btn: App::get()->toolBtns)
    {
        if (btn == L"|") continue;
        if (index == hoverIndex) 
        {
            WinCap::get()->changeDrawState(btn);
            InvalidateRect(hwnd, nullptr, false);
            break;
        }
        index += 1;
    }
}

void WinToolMain::show()
{
    auto win = WinCap::get();
    //win->changeDrawState(L"rect");
    x = static_cast<int>(win->cutMask->maskRect.right - w);
    y = static_cast<int>(win->cutMask->maskRect.bottom + 7 * win->dpi);
    if (y + h > win->h) {
        y = win->cutMask->maskRect.top - 7 * win->dpi - h;
    }
    if (y < win->y) {
        y = win->cutMask->maskRect.bottom - 7 * win->dpi - h;
        x -= 7 * win->dpi;
    }
    SetWindowPos(hwnd, nullptr,x,y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    InvalidateRect(hwnd, nullptr, false);
    ShowWindow(hwnd, SW_SHOW);
}
