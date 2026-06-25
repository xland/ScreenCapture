#include "pch.h"
#include "Lang.h"
#include "ToolSubColor.h"
#include "ToolSub.h"
#include "Shape/ShapeTextWin.h"

ToolSubColor::ToolSubColor(ToolSub* win):win{win}
{
}

ToolSubColor::~ToolSubColor()
{
}

void ToolSubColor::paint()
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
void ToolSubColor::winReady()
{
    btnSize = 23 * win->dpi;
    names = {
        Lang::get(L"color.red"), Lang::get(L"color.yellow"), Lang::get(L"color.green"), Lang::get(L"color.cyan"),
        Lang::get(L"color.blue"), Lang::get(L"color.purple"), Lang::get(L"color.pink"), Lang::get(L"color.black")
    };
    createOneBrush(0XCF1322);
    createOneBrush(0XD48806);
    createOneBrush(0X389E0D);
    createOneBrush(0X13C2C2);
    createOneBrush(0X0958D9);
    createOneBrush(0X722ED1);
    createOneBrush(0XEB2F96);
    createOneBrush(0X000000);
}

bool ToolSubColor::hover(const int& x)
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
            win->showTipAt(win->x + start + indexColor * btnSize + win->btnSize/2, int(win->y + win->marginTop + 4 * win->dpi + 0.5));
        }
        return true;
    }
    return false;
}

bool ToolSubColor::mouseDown(const int& x, const int& y)
{
    if (!visible || x < start|| x > end) return false;
    if (indexSelected != indexHovered) {
        indexSelected = indexHovered;
        ShapeTextWin::get()->changeState();
        return true;
    }
    return false;
}

void ToolSubColor::createOneBrush(const int& color)
{
    ComPtr<ID2D1SolidColorBrush> brush;
    win->render->CreateSolidColorBrush(D2D1::ColorF(color), brush.GetAddressOf());
    brushes.push_back(std::move(brush));
}
D2D1_COLOR_F ToolSubColor::getSelectedColor()
{
    return brushes[indexSelected]->GetColor();
}

void ToolSubColor::setVals(const float& start, bool visible)
{
    this->start = start;
    this->end = win->w;
    this->visible = visible;
}