#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "Win/WinPin.h"
#include "ToolBase.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "History.h"


ToolMain::ToolMain(const int& x, const int& y, const int& w, const int& h, WinPin* parent) : ToolBase(x, y, w, h), parent{parent}
{
    btnId = { L"rect",L"ellipse",L"arrow",L"number",L"line",L"text",L"mosaic", L"eraser",L"undo",L"redo",L"close",L"save",L"clipboard" };
}
ToolMain::~ToolMain()
{
}
void ToolMain::onCreated()
{
    btnSize = 32.f * dpi;
    initTip();
    initBrush();
    initBtn();
}
BOOL ToolMain::onCursor()
{
    setCursor(IDC_HAND);
    return TRUE;
}

void ToolMain::onPaint()
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

void ToolMain::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = L"";
        selectIndex = -1;
        hide();
		return;
	}
    if (hoverIndex < 0) return;
    auto& state = btnId[hoverIndex];
    if (state == L"undo") {
        parent->history->undo();
        return;
    }
    else if (state == L"redo") {
        parent->history->redo();
        return;
    }
    else if (state == L"clipboard") {
        parent->copyToClipboard();
        return;
    }
    else if (state == L"save") {
        parent->saveToFile();
        return;
    }
    else if (state == L"close") {
        parent->close();
        return;
    }
    else {
        if (state == this->state) {
            this->state = L"";
            selectIndex = -1;
            parent->toolSub->hide();
            // 关闭 ToolSub 后，如果处于 TopRight/OverlapBottomRight 布局，
            // ToolMain 需要回到不预留 ToolSub 的位置
            parent->updateToolLayout(false);
        }
        else {
            this->state = state;
            selectIndex = hoverIndex;
            // 先让 ToolMain 挪到给 ToolSub 预留空间的位置，
            // 再由 ToolSub::resetVal 根据 ToolMain 新位置摆放自身
            parent->updateToolLayout(true);
            parent->toolSub->resetVal();
            parent->toolSub->setBorderPath();
            parent->toolSub->show();
            parent->toolSub->refresh();
        }
        refresh();
    }
}
void ToolMain::onMouseMove(const int& x, const int& y)
{
    size_t index = static_cast<int>(x / btnSize);
    if (index >= btnId.size()) index = btnId.size() - 1;
    if (index != hoverIndex) {
        hoverIndex = (int)index;
        tipText = Lang::get(std::format(L"tool.{}", btnId[index]));
        showTipAt((int)(this->x + index * btnSize + btnSize / 2), (int)(this->y + 4 * dpi));
        refresh();
    }
}

void ToolMain::syncHoverWithCursor()
{
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) return;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);

    if (pt.x >= 0 && pt.x < w && pt.y >= 0 && pt.y < h) {
        if (!isMouseIn) {
            isMouseIn = true;
            TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        onMouseMove(pt.x, pt.y);
        refresh();
    }
    else if (isMouseIn || hoverIndex >= 0) {
        isMouseIn = false;
        TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
        tme.dwFlags = TME_CANCEL | TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
        onMouseLeave();
    }
}
void ToolMain::onMouseLeave()
{
    hoverIndex = -1;
    refresh();
    hideTip();
}

void ToolMain::onDpiChanged()
{
    btnSize = 32.f * dpi;
    initBtn();
}
void ToolMain::initBtn()
{
    btnLayout.clear();
    auto fSize{ 14.f * dpi };
    btnLayout.push_back(makeIconLayout(L"\ue8e8", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6bc", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue603", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue776", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue601", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6ec", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue82e", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6be", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ued85", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ued8a", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue608", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6ad", btnSize, btnSize, fSize));
}

void ToolMain::onKeyDown(const UINT& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->redo();
    }
    else if (key == 'C' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->copyToClipboard();
    }
    else if (key == 'S' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->saveToFile();
    }
    else if (key == VK_RETURN)
    {
        parent->copyToClipboard();
    }
    else if (key == VK_DELETE)
    {
        parent->history->removeHoverShape();
    }
    else if (key == VK_ESCAPE)
    {
        parent->close();
    }
}