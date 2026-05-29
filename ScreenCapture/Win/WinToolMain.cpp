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

WinToolMain::WinToolMain(const int& x, const int& y, const int& w, const int& h) : WinToolBase(x, y, w, h)
{
    btnSize = (float)h;
	marginTop = 0.f;
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
	    auto y = winPin->y + winPin->h + 4.f * winPin->dpi;
	    winToolMain = std::make_unique<WinToolMain>((int)x, (int)y, (int)w, (int)h);
	    winToolMain->createWindow(WS_EX_TOOLWINDOW | WS_EX_TOPMOST| WS_EX_NOACTIVATE);
        winToolMain->initTip();
        winToolMain->initBrush();
		winToolMain->initBtn();
        winToolMain->show();
    }
    else {
        auto winPin = WinPin::getCur();
        auto win = winToolMain.get();
        auto x = winPin->x;
        auto y = (int)(winPin->y + winPin->h + 3.f * winPin->dpi);
        win->move(x, y);
        win->show();
        if(win->state != "")
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
    for (auto& layout : btnLayout)
    {
		paintIcon(btnSize * btnIndex, layout.Get(), btnIndex == hoverIndex, btnIndex == selectIndex);
        btnIndex += 1;
    }
    for (auto index: spliterIndex)
    {
        auto lx{ (float)btnSize * index }, lt{ 2.f * margin };
        render->DrawLine({ lx,lt }, { lx,btnSize - lt }, brushSpliter.Get(), 0.8f);
    }
    auto r = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawRectangle(r, brushSpliter.Get(), 2 * dpi);
}

void WinToolMain::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = "";
        selectIndex = -1;
		WinToolMain::get()->hide();
		return;
	}
    auto& state = btnId[hoverIndex];
    if (state == "undo") {
        return;
    }
    else if (state == "redo") {
        return;
    }
    else if (state == "clipboard") {
        return;
    }
    else if (state == "save") {
        return;
    }
    else if (state == "close") {
        App::get()->exit(1);
        return;
    }
    else {
        if (state == this->state) {
            this->state = "";
            selectIndex = -1;
            WinToolSub::get()->hide();
        }
        else {
            this->state = state;
            selectIndex = hoverIndex;
            WinToolSub::get()->popup();
        }
        refresh();
    }
}
void WinToolMain::onMouseMove(const int& x, const int& y)
{
    size_t index = static_cast<int>(x / btnSize);
    if (index >= btnId.size()) index = btnId.size() - 1;
    if (index != hoverIndex) {
        hoverIndex = (int)index;
        tipText = btnName[index];
        showTipAt((int)(this->x + index * btnSize + btnSize / 2), (int)(this->y + marginTop + 4 * dpi));
        refresh();
    }
}
void WinToolMain::onMouseLeave()
{
    hoverIndex = -1;
    refresh();
    hideTip();
}

void WinToolMain::initBtn()
{
    btnId = { "rect" , "ellipse", "arrow", "number" , "line" ,"text" , "mosaic", "eraser", "undo", "redo", "pin", "clipboard" , "save" , "close" };
    btnName = { L"矩形",L"圆形",L"箭头",L"标号",L"线条",L"文本",L"马赛克",L"橡皮擦",L"撤销",L"重做",L"钉住",L"剪切板",L"保存",L"关闭" };
    auto fontSize{ 14.f * dpi };
    btnLayout = {
        getIconLayout(L"\ue8e8", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue6bc", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue603", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue776", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue601", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue6ec", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue82e", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue6be", fontSize, btnSize, btnSize),
        getIconLayout(L"\ued85", fontSize, btnSize, btnSize),
        getIconLayout(L"\ued8a", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue6a2", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue650", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue608", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue62d", fontSize, btnSize, btnSize),
    };
}
