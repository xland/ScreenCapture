#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeEraser.h"

ShapeEraser::ShapeEraser(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }
{
	auto toolSub = WinToolSub::get();
	win->render->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	strokeWidth = toolSub->sliderVal;
	isFill = toolSub->selectIndex == 0;
}

ShapeEraser::~ShapeEraser()
{

}

void ShapeEraser::paint()
{
	if (isFill) {
		win->render->FillRectangle(rect, brush.Get());
	}
	else {
		win->render->DrawRectangle(rect, brush.Get(), strokeWidth);
	}
}

void ShapeEraser::paintDragger()
{
	for (auto& dragger:draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeEraser::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	
}

void ShapeEraser::mouseDown(const float& x, const float& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		pressX = x;
		pressY = y;
		hoverDraggerIndex = 4;
	}
	else if (hoverDraggerIndex == 0) {
		pressX = rect.right;
		pressY = rect.bottom;
	}
	else if (hoverDraggerIndex == 1) {
		pressY = rect.bottom;
	}
	else if (hoverDraggerIndex == 2) {
		pressX = rect.left;
		pressY = rect.bottom;
	}
	else if (hoverDraggerIndex == 3) {
		pressX = rect.left;
	}
	else if (hoverDraggerIndex == 4) {
		pressX = rect.left;
		pressY = rect.top;
	}
	else if (hoverDraggerIndex == 5) {
		pressY = rect.top;
	}
	else if (hoverDraggerIndex == 6) {
		pressY = rect.top;
		pressY = rect.right;
	}
	else if (hoverDraggerIndex == 7) {
		pressX = rect.right;
	}
	else if (hoverDraggerIndex == 8) {
		pressX = x - rect.left;
		pressY = y - rect.top;
	}
}

void ShapeEraser::mouseUp(const float& x, const float& y)
{
	auto half{ draggerSize / 2 }, w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
	draggers[0].left = rect.left - half;
	draggers[0].top = rect.top - half;
	draggers[0].right = rect.left + half;
	draggers[0].bottom = rect.top + half;

	draggers[1].left = rect.left + w / 2 - half;
	draggers[1].top = rect.top - half;
	draggers[1].right = rect.left + w / 2 + half;
	draggers[1].bottom = rect.top + half;


	draggers[2].left = rect.right - half;
	draggers[2].top = rect.top - half;
	draggers[2].right = rect.right + half;
	draggers[2].bottom = rect.top + half;


	draggers[3].left = rect.right - half;
	draggers[3].top = rect.top + h / 2 - half;
	draggers[3].right = rect.right + half;
	draggers[3].bottom = rect.top + h / 2 + half;


	draggers[4].left = rect.right - half;
	draggers[4].top = rect.bottom - half;
	draggers[4].right = rect.right + half;
	draggers[4].bottom = rect.bottom + half;


	draggers[5].left = rect.left + w / 2 - half;
	draggers[5].top = rect.bottom - half;
	draggers[5].right = rect.left + w / 2 + half;
	draggers[5].bottom = rect.bottom + half;

	draggers[6].left = rect.left - half;
	draggers[6].top = rect.bottom - half;
	draggers[6].right = rect.left + half;
	draggers[6].bottom = rect.bottom + half;

	draggers[7].left = rect.left - half;
	draggers[7].top = rect.top + h / 2 - half;
	draggers[7].right = rect.left + half;
	draggers[7].bottom = rect.top + h / 2 + half;
}

void ShapeEraser::mouseMove(const float& x, const float& y)
{
    hoverDraggerIndex = -1;
    if (Util::isInRect(draggers[0], x, y))
    {
        hoverDraggerIndex = 0;
    }
    else if (Util::isInRect(draggers[1], x, y))
    {
        hoverDraggerIndex = 1;
    }
    else if (Util::isInRect(draggers[2], x, y))
    {
        hoverDraggerIndex = 2;
    }
    else if (Util::isInRect(draggers[3], x, y))
    {
        hoverDraggerIndex = 3;
    }
    else if (Util::isInRect(draggers[4], x, y))
    {
        hoverDraggerIndex = 4;
    }
    else if (Util::isInRect(draggers[5], x, y))
    {
        hoverDraggerIndex = 5;
    }
    else if (Util::isInRect(draggers[6], x, y))
    {
        hoverDraggerIndex = 6;
    }
    else if (Util::isInRect(draggers[7], x, y))
    {
        hoverDraggerIndex = 7;
    }
    if (hoverDraggerIndex == -1)
    {
        auto half{ strokeWidth / 2.f+win->dpi };//多个一个dpi，让范围更大点
        if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half ) 
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
        }
    }
}

void ShapeEraser::setCursor()
{
	if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4) {
		win->setCursor(IDC_SIZENWSE);
	}
	else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
		win->setCursor(IDC_SIZENS);
	}
	else if (hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
		win->setCursor(IDC_SIZENESW);
	}
	else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
		win->setCursor(IDC_SIZEWE);
	}
	else if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
}
