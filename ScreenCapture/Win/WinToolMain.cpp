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


WinToolMain::WinToolMain(const int& x, const int& y, const int& w, const int& h, WinPin* parent) : WinToolBase(x, y, w, h,parent)
{
	marginTop = 0.f;
}
WinToolMain::~WinToolMain()
{
}
BOOL WinToolMain::onCursor()
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
		paintIcon(parent->toolBtnSize * btnIndex, layout.Get(), btnIndex == hoverIndex, btnIndex == selectIndex);
        btnIndex += 1;
    }
    for (auto index: spliterIndex)
    {
        auto lx{ (float)parent->toolBtnSize * index }, lt{ 2.f * margin };
        render->DrawLine({ lx,lt }, { lx,parent->toolBtnSize - lt }, brushSpliter.Get(), 0.8f);
    }
    auto r = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawRectangle(r, brushSpliter.Get(), 2 * dpi);
}

void WinToolMain::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = "";
        selectIndex = -1;
        hide();
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
            parent->toolSub->hide();
        }
        else {
            this->state = state;
            selectIndex = hoverIndex;
            parent->toolSub->resetVal();
            parent->toolSub->setBorderPath();
            parent->toolSub->show();
            parent->toolSub->refresh();
        }
        refresh();
    }
}
void WinToolMain::onMouseMove(const int& x, const int& y)
{
    size_t index = static_cast<int>(x / parent->toolBtnSize);
    if (index >= btnId.size()) index = btnId.size() - 1;
    if (index != hoverIndex) {
        hoverIndex = (int)index;
        tipText = btnName[index];
        showTipAt((int)(this->x + index * parent->toolBtnSize + parent->toolBtnSize / 2), (int)(this->y + marginTop + 4 * dpi));
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