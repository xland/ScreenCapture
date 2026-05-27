#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "ShapeRect.h"

ShapeRect::ShapeRect(WinPin* win):ShapeBase(win)
{
	auto toolSub = WinToolSub::get();
	win->render->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	auto toolMain = WinToolMain::get();
	strokeWidth = 3.f;
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
	state = ShapeState::ready;
	return true;
}
