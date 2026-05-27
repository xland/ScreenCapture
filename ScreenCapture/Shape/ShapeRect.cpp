#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeRect.h"

ShapeRect::ShapeRect(WinPin* win):ShapeBase(win)
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
	auto [left, right] = std::minmax(pressX, xf);
	auto [top, bottom] = std::minmax(pressY, yf);
	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;
}

bool ShapeRect::mouseDown(const int& x, const int& y)
{
	pressX = x;
	pressY = y;
	return true;
}

bool ShapeRect::mouseUp(const int& x, const int& y)
{
	if (draggers.empty()) {
		auto half{ draggerSize / 2 }, w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
		draggers.push_back(D2D1::RectF(rect.left - half, rect.top - half, rect.left +half, rect.top + half));
		draggers.push_back(D2D1::RectF(rect.left+w/2 - half, rect.top - half, rect.left+w/2 + half, rect.top + half));
		draggers.push_back(D2D1::RectF(rect.right - half, rect.top - half, rect.right + half, rect.top + half));
		draggers.push_back(D2D1::RectF(rect.right - half, rect.top + h/2 - half, rect.right + half, rect.top+h/2 + half));
		draggers.push_back(D2D1::RectF(rect.right - half, rect.bottom - half, rect.right + half, rect.bottom + half));
		draggers.push_back(D2D1::RectF(rect.left+w/2 - half, rect.bottom - half, rect.left+w/2 + half, rect.bottom + half));
		draggers.push_back(D2D1::RectF(rect.left - half, rect.bottom - half, rect.left + half, rect.bottom + half));
		draggers.push_back(D2D1::RectF(rect.left - half, rect.top+h/2 - half, rect.left + half, rect.top+h/2 + half));
	}
	for (size_t i = 0; i < draggers.size(); i++)
	{
		auto flag = Util::isInRect(draggers[i], x, y);
		if (flag) {
            hoverDraggerIndex = i;
			break;
		}
	}
	state = ShapeState::ready;
	return true;
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
