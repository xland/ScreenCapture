#include "pch.h"
#include "ShapeArrow.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"

ShapeArrow::ShapeArrow(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }
{
	auto toolSub = WinToolSub::get();
	win->render->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	strokeWidth = toolSub->sliderVal;
	isFill = toolSub->selectIndex == 0;
}

ShapeArrow::~ShapeArrow()
{

}

void ShapeArrow::paint()
{
	if (isFill) {
		win->render->FillGeometry(path.Get(), brush.Get());
	}
	else {
		win->render->DrawGeometry(path.Get(), brush.Get(),strokeWidth);
	}
}

void ShapeArrow::paintDragger()
{
	for (auto& dragger : draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeArrow::mouseDrag(const int& x, const int& y)
{
	auto xf = static_cast<float>(x);
	auto yf = static_cast<float>(y);
	if (hoverDraggerIndex == 0) {
	}
	else if (hoverDraggerIndex == 1) {
	}
	else if (hoverDraggerIndex == 8) {
	}
}

void ShapeArrow::mouseDown(const int& x, const int& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		startX = (float)x;
		startY = (float)y;
		hoverDraggerIndex = 4;
	}
	else if (hoverDraggerIndex == 0) {
		endX = (float)x;
		endY = (float)y;
	}
	else if (hoverDraggerIndex == 1) {
		startX = (float)x;
		startY = (float)y;
	}
	else if (hoverDraggerIndex == 8) {
		pressX = (float)x - startX;
		pressY = (float)y - startY;
	}
}

void ShapeArrow::mouseUp(const int& x, const int& y)
{
	auto half{ draggerSize / 2 };
	draggers[0].left = startX - half;
	draggers[0].top = startY - half;
	draggers[0].right = startX + half;
	draggers[0].bottom = startY + half;

	draggers[1].left = endX - half;
	draggers[1].top = endY - half;
	draggers[1].right = endX + half;
	draggers[1].bottom = endY + half;
}

void ShapeArrow::mouseMove(const int& x, const int& y)
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
	if (hoverDraggerIndex == -1)
	{
	}
}

void ShapeArrow::setCursor()
{
	win->setCursor(IDC_SIZEALL);
}
