#include "pch.h"
#include "WinToolSubColor.h"
#include "WinToolSub.h"
#include "Shape/ShapeTextWin.h"

WinToolSubColor::WinToolSubColor(WinToolSub* win):win{win}
{
}

WinToolSubColor::~WinToolSubColor()
{
}

void WinToolSubColor::paint()
{
	if (!visible) return;
    int index{ 0 };
    D2D1_POINT_2F origin = { start,win->marginTop };
    for (auto& brush : brushes)
    {
        auto icon = index == indexSelected ? win->getBtnIconLayout("check") : win->getBtnIconLayout("uncheck");
        if (index == indexHovered || index == indexSelected) {
            float paddingTopBottom{ 6.f * win->dpi }, paddingLeftRight{ 5.f * win->dpi }, borderRadius{5.6f*win->dpi};
            D2D1_ROUNDED_RECT rr = { { origin.x + paddingLeftRight, paddingTopBottom + win->marginTop, origin.x + win->btnSize - paddingLeftRight, win->h - paddingTopBottom }, borderRadius, borderRadius };
            win->render->FillRoundedRectangle(rr, win->brushSelect.Get());
        }
        win->render->DrawTextLayout(origin, icon, brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        origin.x += btnSize;
        index += 1;
    }
}
void WinToolSubColor::winReady()
{
    btnSize = 23 * win->dpi;
    names = { L"红",L"黄",L"绿",L"青",L"蓝",L"紫",L"粉",L"黑"};
    createOneBrush(0XCF1322);
    createOneBrush(0XD48806);
    createOneBrush(0X389E0D);
    createOneBrush(0X13C2C2);
    createOneBrush(0X0958D9);
    createOneBrush(0X722ED1);
    createOneBrush(0XEB2F96);
    createOneBrush(0X000000);
}

bool WinToolSubColor::hover(const int& x)
{
    if (!visible) return false;
    int indexColor;
    if (x > start && x < end) {
        indexColor = static_cast<int>((x - start) / btnSize);
        if (indexColor >= names.size()) indexColor = (int)names.size() - 1;
    }
    else {
        indexColor = -1;
    }
    if (indexColor != indexHovered) {
        indexHovered = indexColor;
        if (indexColor >= 0) {
            win->tipText = names[indexColor];
            win->showTipAt(int(win->x + start + indexColor * btnSize + btnSize / 2 + 0.5), int(win->y + win->marginTop + 4 * win->dpi + 0.5));
        }
        return true;
    }
    return false;
}

bool WinToolSubColor::mouseDown(const int& x, const int& y)
{
    if (!visible || x < start|| x > end) return false;
    if (indexSelected != indexHovered) {
        indexSelected = indexHovered;
        ShapeTextWin::get()->changeState();
        return true;
    }
    return false;
}

void WinToolSubColor::createOneBrush(const int& color)
{
    ComPtr<ID2D1SolidColorBrush> brush;
    win->render->CreateSolidColorBrush(D2D1::ColorF(color), brush.GetAddressOf());
    brushes.push_back(std::move(brush));
}
D2D1_COLOR_F WinToolSubColor::getSelectedColor()
{
    return brushes[indexSelected]->GetColor();
}

void WinToolSubColor::setVals(const float& start, bool visible)
{
    this->start = start;
    this->end = win->w;
    this->visible = visible;
}