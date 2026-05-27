#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeRect.h"

ShapeRect::ShapeRect(WinPin* win) :ShapeBase(win), draggers{ 
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
}

ShapeRect::~ShapeRect()
{

}

void ShapeRect::paint()
{
	win->render->DrawRectangle(rect, brush.Get(), strokeWidth);
}

void ShapeRect::paintDragger()
{
	if (state != ShapeState::ready) return;	
	for (auto& dragger:draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeRect::mouseDrag(const int& x, const int& y)
{
	auto xf = static_cast<float>(x);
	auto yf = static_cast<float>(y);
	if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4) {
		auto [left, right] = std::minmax(pressX, xf);
		auto [top, bottom] = std::minmax(pressY, yf);
		rect.left = left;
		rect.right = right;
		rect.top = top;
		rect.bottom = bottom;
	}
	else if (hoverDraggerIndex == 1) {
		auto [top, bottom] = std::minmax(pressY, yf);
		rect.top = top;
		rect.bottom = bottom;
	}
	else if (hoverDraggerIndex == 1) {
		auto [top, bottom] = std::minmax(pressY, yf);
		rect.top = top;
		rect.bottom = bottom;
	}
}

void ShapeRect::mouseDown(const int& x, const int& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		pressX = (float)x;
		pressY = (float)y;
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
		pressX = (float)x;
		pressY = (float)y;
	}
}

void ShapeRect::mouseUp(const int& x, const int& y)
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

	state = ShapeState::ready;
}

void ShapeRect::mouseMove(const int& x, const int& y)
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
        auto half{ strokeWidth / 2.f };
        if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half ) 
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
        }
    }
}

void ShapeRect::setCursor()
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
