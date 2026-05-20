#include "pch.h"
#include "Util.h"
#include "WinToolMain.h"
#include "App.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinPin.h"

std::unique_ptr<WinToolMain> winToolMain;

WinToolMain::WinToolMain(const int& x, const int& y, const int& w, const int& h) : WinBase(x,y,w,h)
{

}
WinToolMain::~WinToolMain()
{
}

void WinToolMain::popup()
{
	if (!winToolMain.get()){
        auto winPin = WinPin::getCur();
        if (!winPin) {
			log(L"not find winPin when create toolMain");
            return;
        }
        auto btnW = 36.f * winPin->dpi;
        auto btnH = 32.f * winPin->dpi;
        auto btnPadding = 3.f * winPin->dpi;
        auto w = btnW * 14;
        auto h = btnH;
	    auto x = winPin->x;
	    auto y = winPin->y + winPin->h + btnPadding;
	    winToolMain = std::make_unique<WinToolMain>(x, y, w, h);
	    winToolMain->btnW = btnW;
        winToolMain->btnH = btnH;
	    winToolMain->btnPadding = btnPadding;
	    winToolMain->createWindow(WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
		winToolMain->initRes();
        winToolMain->show();
    }
    else {
        auto winPin = WinPin::getCur();
        auto win = winToolMain.get();
        auto x = winPin->x;
        auto y = winPin->y + winPin->h + win->btnPadding;
        win->move(x, y);
        win->show();
    }
}
bool WinToolMain::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}
void WinToolMain::initRes()
{
    render->CreateSolidColorBrush(D2D1::ColorF(0xe6f4ff), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x333333), brushIcon.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushIconHover.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0xbbbbbb), brushSpliter.GetAddressOf());
    auto fontSize{ 14.f * dpi };
    btnIcons.insert({ L"rect" ,getIconLayout(L"\ue8e8",fontSize,btnW,h) });
    btnIcons.insert({ L"ellipse" ,getIconLayout(L"\ue6bc",fontSize,btnW,h) });
    btnIcons.insert({ L"arrow" ,getIconLayout(L"\ue603",fontSize,btnW,h) });
    btnIcons.insert({ L"number" ,getIconLayout(L"\ue776",fontSize,btnW,h) });
    btnIcons.insert({ L"line" ,getIconLayout(L"\ue601",fontSize,btnW,h) });
    btnIcons.insert({ L"text" ,getIconLayout(L"\ue6ec",fontSize,btnW,h) });
    btnIcons.insert({ L"mosaic" ,getIconLayout(L"\ue82e",fontSize,btnW,h) });
    btnIcons.insert({ L"eraser" ,getIconLayout(L"\ue6be",fontSize,btnW,h) });
    btnIcons.insert({ L"undo" ,getIconLayout(L"\ued85",fontSize,btnW,h) });
    btnIcons.insert({ L"redo" ,getIconLayout(L"\ued8a",fontSize,btnW,h) });
    btnIcons.insert({ L"pin" ,getIconLayout(L"\ue6a2",fontSize,btnW,h) });
    btnIcons.insert({ L"clipboard" ,getIconLayout(L"\ue650",fontSize,btnW,h) });
    btnIcons.insert({ L"save" ,getIconLayout(L"\ue608",fontSize,btnW,h) });
    btnIcons.insert({ L"close" ,getIconLayout(L"\ue62d",fontSize,btnW,h) });
}

void WinToolMain::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF));
    int btnIndex{ 0 };
    for (auto& pair : btnIcons)
    {
        if (btnIndex == hoverIndex || pair.first == L"drawState") {
            auto xStart{ btnW * btnIndex };
            D2D1_ROUNDED_RECT rr = { { xStart + btnPadding, btnPadding, xStart + btnW - btnPadding, h - btnPadding }, 6, 6 };
            render->FillRoundedRectangle(rr, brushBg.Get());
            D2D1_POINT_2F origin = { xStart, 0.f };
            render->DrawTextLayout(origin, pair.second.Get(), brushIconHover.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        else {
            D2D1_POINT_2F origin = { btnW * btnIndex, 0.f };
            render->DrawTextLayout(origin, pair.second.Get(), brushIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        btnIndex += 1;
    }
    //auto lx{ btnW * btnIndex }, lt{ 3.f * btnPadding };
    //render->DrawLine({ lx,lt }, { lx,btnH - lt }, brushSpliter.Get(), 0.8f);
}

void WinToolMain::onMouseDown(const int& x, const int& y, bool isRight)
{
    //auto index{ 0 };
    //for (auto& btn: App::get()->toolBtns)
    //{
    //    if (btn == L"|") continue;
    //    if (index == hoverIndex) 
    //    {
    //        InvalidateRect(hwnd, nullptr, false);
    //        break;
    //    }
    //    index += 1;
    //}
}
