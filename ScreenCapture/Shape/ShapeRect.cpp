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