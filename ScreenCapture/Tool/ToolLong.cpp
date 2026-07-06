#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "Win/WinLong.h"
#include "ToolBase.h"
#include "ToolLong.h"
#include "ToolSub.h"
#include "History.h"


ToolLong::ToolLong(const int& x, const int& y, const int& w, const int& h, WinLong* parent) : ToolBase(x, y, w, h), parent{ parent }
{
    btnId = { L"pin" , L"close" , L"save" , L"clipboard" };
}
ToolLong::~ToolLong()
{
}
void ToolLong::onCreated()
{
    btnSize = 32.f * dpi;
    initTip();
    initBrush();
    auto fSize{ 14.f * dpi };
    btnLayout.push_back(App::makeIconLayout(L"\ue6a2", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue608", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue6ad", btnSize, btnSize, fSize));
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
    for (auto& layout : btnLayout)
    {
		paintIcon(btnSize * btnIndex, layout.Get(), btnIndex == hoverIndex, btnIndex == selectIndex);
        btnIndex += 1;
    }
}

void ToolLong::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = L"";
        selectIndex = -1;
        hide();
		return;
	}
    if (hoverIndex < 0) return;
    auto& state = btnId[hoverIndex];
    if (state == L"pin") {
        parent->pin();
    }
    else if (state == L"clipboard") {
        parent->copyToClipboard();
    }
    else if (state == L"save") {
        parent->saveToFile();
    }
    close();
    WinLong::release();
}
void ToolLong::onMouseMove(const int& x, const int& y)
{
    size_t index = static_cast<int>(x / btnSize);
    if (index >= btnId.size()) index = btnId.size() - 1;
    if (index != hoverIndex) {
        hoverIndex = (int)index;
        auto& str = Lang::get(std::format(L"tool.{}", btnId[index]));
        showTipAt((int)(this->x + index * btnSize + btnSize / 2), (int)(this->y + 4 * dpi),str);
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
    auto fSize{ 14.f * dpi };
    btnLayout.push_back(App::makeIconLayout(L"\ued8a", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue650", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue608", btnSize, btnSize, fSize));
    btnLayout.push_back(App::makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));
}