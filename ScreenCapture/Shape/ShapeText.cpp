#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeText.h"
#include "ShapeTextWin.h"

ShapeText::ShapeText(WinPin* win) :ShapeBase(win)
{
	auto toolSub = WinToolSub::get();
	color = toolSub->getSelectedColor();
	fontSize = toolSub->sliderVal;
	isBold = toolSub->selectIndex == 0;
	isItalic = toolSub->selectIndex == 1;
}

ShapeText::~ShapeText()
{

}

void ShapeText::paint()
{
	auto textWin = ShapeTextWin::get();
	POINT pos{ textWin->x, textWin->y };
	ScreenToClient(win->hwnd, &pos);
	if (isEditing) {
		auto borderPadding{ 6.f * win->dpi };
		auto r = D2D1::RectF(pos.x-borderPadding, pos.y-borderPadding, pos.x+textWin->w + borderPadding, pos.y+textWin->h + borderPadding);
		win->render->DrawRectangle(r, textBrush.Get(), win->dpi, textWin->dashedStrokeStyle.Get());
	}
	win->render->DrawTextLayout({ (float)pos.x, (float)pos.y }, textLayout.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
}

void ShapeText::paintDragger()
{
}

void ShapeText::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
}

void ShapeText::mouseDown(const float& x, const float& y)
{
	pressX = x;
	pressY = y;
	if (hoverDraggerIndex == -1) { //首次创建
		auto textWin = ShapeTextWin::get();
		textWin->setShape(this);
		isEditing = true;
		win->refresh();
	}
}

void ShapeText::mouseUp(const float& x, const float& y)
{

}

void ShapeText::mouseMove(const float& x, const float& y)
{
    if (Util::isInRect(rect, x, y))
    {
		hoverDraggerIndex = 8;
    }
}

void ShapeText::setCursor()
{
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_IBEAM);
	}
}

void ShapeText::finishEdit()
{

}


