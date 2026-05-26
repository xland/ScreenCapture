#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "ShapeRect.h"

ShapeRect::ShapeRect(WinPin* win):ShapeBase(win)
{
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x990022), brush.GetAddressOf());
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
	auto [left, right] = std::minmax(pressX, (float)x);
	auto [top, bottom] = std::minmax(pressY, (float)y);
	rect = D2D1::RectF(left, top, right, bottom);
}

bool ShapeRect::mouseDown(const int& x, const int& y)
{
	pressX = x;
	pressY = y;
	return true;
}