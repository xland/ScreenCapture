#include "pch.h"
#include "Util.h"
#include "App.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinPin.h"
#include "WinToolBase.h"
#include "WinToolMain.h"
#include "WinToolSub.h"

std::unique_ptr<WinToolMain> winToolMain;

WinToolMain::WinToolMain(const int& x, const int& y, const int& w, const int& h) : WinToolBase(x,y,w,h)
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
        auto btnSize{32.f * winPin->dpi}; //todo dpi change
		auto w = btnSize * 14; //主工具条一共有14个按钮；todo dpi change
        auto h = btnSize;
	    auto x = winPin->x;
	    auto y = winPin->y + winPin->h + 3.f * winPin->dpi;
	    winToolMain = std::make_unique<WinToolMain>(x, y, w, h);
	    winToolMain->createWindow(WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
        winToolMain->initBrush();
		winToolMain->addBtns({ {L"rect" ,L"\ue8e8"},
        {L"ellipse" ,L"\ue6bc"},
        {L"arrow" ,L"\ue603"},
        {L"number" ,L"\ue776"},
        {L"line" ,L"\ue601"} ,
        {L"text" ,L"\ue6ec"},
        {L"mosaic" ,L"\ue82e"},
        {L"eraser" ,L"\ue6be"},
        {L"undo" ,L"\ued85"},
        {L"redo" ,L"\ued8a"},
        {L"pin" ,L"\ue6a2" },
        {L"clipboard" ,L"\ue650"},
        {L"save" ,L"\ue608"},
        {L"close" ,L"\ue62d"} });
        winToolMain->show();
        WinToolSub::popup();
    }
    else {
        auto winPin = WinPin::getCur();
        auto win = winToolMain.get();
        auto x = winPin->x;
        auto y = winPin->y + winPin->h + 3.f * winPin->dpi;
        win->move(x, y);
        win->show();
        if(win->state != L"")
        {
            WinToolSub::popup();
        }
    }
}
WinToolMain* WinToolMain::get()
{
    return winToolMain.get();
}
bool WinToolMain::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}

void WinToolMain::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF));
    int btnIndex{ 0 };
    auto margin{ 4.f * dpi };
    for (auto& pair : btnIcons)
    {
        auto xStart{ h * btnIndex };
        D2D1_POINT_2F origin = { xStart, 0.f };
        if(btnIndex == selectIndex) {
			D2D1_ROUNDED_RECT rr = { { xStart+2*dpi, margin, xStart + h-2*dpi, h - margin }, 8, 8 };
			render->FillRoundedRectangle(rr, brushSelect.Get());
            render->DrawTextLayout(origin, pair.second.Get(), brushBlue.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        else {
            auto brush = btnIndex == hoverIndex ? brushBlue.Get() : brushIcon.Get();
            render->DrawTextLayout(origin, pair.second.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        btnIndex += 1;
    }
    for (auto index: spliterIndex)
    {
        auto lx{ (float)h * index }, lt{ 2.f * margin };
        render->DrawLine({ lx,lt }, { lx,h - lt }, brushSpliter.Get(), 0.8f);
    }
    auto r = D2D1::RectF(0, 0, w, h);
    render->DrawRectangle(r, brushSpliter.Get(), 2 * dpi);
}

void WinToolMain::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
		WinToolMain::get()->hide();
		return;
	}
    auto index{ 0 };
    for (auto& btn: btnIcons)
    {
        if (index == hoverIndex) 
        {
            if (index == selectIndex) {
                state = L"";
                selectIndex = -1;
            }
            else {
                state = btn.first;
				selectIndex = index;
            }
            refresh();
			WinToolSub::get()->changeState();
            break;
        }
        index += 1;
    }
}
void WinToolMain::onMouseMove(const int& x, const int& y)
{
    auto index = static_cast<int>(x / h);
    if (index != hoverIndex) {
        hoverIndex = index;
        refresh();
    }
}
void WinToolMain::onMouseLeave()
{
    hoverIndex = -1;
    InvalidateRect(hwnd, nullptr, false);
}