#include "pch.h"
#include "Util.h"
#include "App.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinPin.h"
#include "WinToolBase.h"
#include "WinToolMain.h"
#include "WinToolSub.h"
#include "History.h"

std::unique_ptr<WinToolMain> winToolMain;

WinToolMain::WinToolMain(const int& x, const int& y, const int& w, const int& h) : WinToolBase(x, y, w, h)
{
    btnSize = (float)h;
	marginTop = 0.f;
}
WinToolMain::~WinToolMain()
{
}

void WinToolMain::popup(WinPin* parent)
{
	if (!winToolMain.get()){
        auto btnSize{32.f * parent->dpi}; //todo dpi change
		auto w = btnSize * 13; //主工具条一共有13个按钮；todo dpi change
        auto h = btnSize;
	    auto x = parent->x;
	    auto y = parent->y + parent->h + 4.f * parent->dpi;
	    winToolMain = std::make_unique<WinToolMain>((int)x, (int)y, (int)w, (int)h);
        winToolMain->parent = parent;
	    winToolMain->createWindow(WS_EX_TOPMOST| WS_EX_NOACTIVATE);
        winToolMain->initTip();
        winToolMain->initBrush();
		winToolMain->initBtn();
        winToolMain->show();
    }
    else {
        auto win = winToolMain.get();
        win->parent = parent;
        auto x = parent->x;
        auto y = (int)(parent->y + parent->h + 3.f * parent->dpi);
        win->move(x, y);
        win->show();
        if(win->state != "")
        {
            WinToolSub::popup(parent);
        }
    }
}
WinToolMain* WinToolMain::get()
{
    return winToolMain.get();
}
bool WinToolMain::onCursor()
{
    setCursor(IDC_HAND);
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
    if (hoverIndex < 0) return;
    auto& state = btnId[hoverIndex];
    if (state == "undo") {
        parent->history->undo();
        return;
    }
    else if (state == "redo") {
        parent->history->redo();
        return;
    }
    else if (state == "clipboard") {
        parent->copyToClipboard();
        return;
    }
    else if (state == "save") {
        parent->saveToFile();
        return;
    }
    else if (state == "close") {
        parent->close();
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
            WinToolSub::get()->popup(parent);
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
    btnId = { "rect" , "ellipse", "arrow", "number" , "line" ,"text" , "mosaic", "eraser", "undo", "redo", "clipboard" , "save" , "close" };
    btnName = { L"矩形",L"圆形",L"箭头",L"标号",L"线条",L"文本",L"马赛克",L"橡皮擦",L"撤销",L"重做",L"剪切板",L"保存",L"关闭" };
    addIconLayout(L"\ue8e8");
    addIconLayout(L"\ue6bc");
    addIconLayout(L"\ue603");
    addIconLayout(L"\ue776");
    addIconLayout(L"\ue601");
    addIconLayout(L"\ue6ec");
    addIconLayout(L"\ue82e");
    addIconLayout(L"\ue6be");
    addIconLayout(L"\ued85");
    addIconLayout(L"\ued8a");
    addIconLayout(L"\ue650");
    addIconLayout(L"\ue608");
    addIconLayout(L"\ue62d");
}