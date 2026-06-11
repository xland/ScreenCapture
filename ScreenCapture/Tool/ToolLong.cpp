#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Win/WinCap.h"
#include "Win/WinLong.h"
#include "ToolBase.h"
#include "ToolLong.h"
#include "ToolSub.h"
#include "History.h"


ToolLong::ToolLong(const int& x, const int& y, const int& w, const int& h, WinLong* parent) : ToolBase(x, y, w, h), parent{ parent }
{
    btnId = { "pin", "clipboard" , "save" , "close" };
    btnName = { L"钉住",L"剪切板",L"保存",L"关闭" };
}
ToolLong::~ToolLong()
{
}
void ToolLong::onCreated()
{
    btnSize = 32.f * dpi;
    addIconLayout(L"\ued8a");
    addIconLayout(L"\ue650");
    addIconLayout(L"\ue608");
    addIconLayout(L"\ue62d");
}
BOOL ToolLong::onCursor()
{
    setCursor(IDC_HAND);
    return TRUE;
}

void ToolLong::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF));
    int btnIndex{ 0 };
    auto margin{ 4.f * dpi };
    for (auto& layout : btnLayout)
    {
		paintIcon(btnSize * btnIndex, layout.Get(), btnIndex == hoverIndex, btnIndex == selectIndex);
        btnIndex += 1;
    }
    auto r = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawRectangle(r, brushSpliter.Get(), 2 * dpi);
}

void ToolLong::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = "";
        selectIndex = -1;
        hide();
		return;
	}
    if (hoverIndex < 0) return;
    auto& state = btnId[hoverIndex];
    //if (state == "pin") {
    //    parent->copyToClipboard();
    //    return;
    //}
    //else if (state == "clipboard") {
    //    parent->copyToClipboard();
    //    return;
    //}
    //else if (state == "save") {
    //    parent->saveToFile();
    //    return;
    //}
    //else if (state == "close") {
    //    parent->toolSub->close();
    //    parent->close();
    //    return;
    //}
}
void ToolLong::onMouseMove(const int& x, const int& y)
{
    size_t index = static_cast<int>(x / btnSize);
    if (index >= btnId.size()) index = btnId.size() - 1;
    if (index != hoverIndex) {
        hoverIndex = (int)index;
        tipText = btnName[index];
        showTipAt((int)(this->x + index * btnSize + btnSize / 2), (int)(this->y + 4 * dpi));
        refresh();
    }
}
void ToolLong::onMouseLeave()
{
    hoverIndex = -1;
    refresh();
    hideTip();
}

void ToolLong::onDpiChanged()
{
    btnSize = 32.f * dpi;
    btnLayout.clear();
    addIconLayout(L"\ued8a");
    addIconLayout(L"\ue650");
    addIconLayout(L"\ue608");
    addIconLayout(L"\ue62d");
}