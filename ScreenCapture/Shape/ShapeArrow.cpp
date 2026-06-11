#include "pch.h"
#include "ShapeArrow.h"
#include "App.h"
#include "Win/WinPin.h"
#include "Tool/WinToolMain.h"
#include "Tool/WinToolSub.h"
#include "Tool/WinToolSubSlider.h"
#include "Util.h"

ShapeArrow::ShapeArrow(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }
{
	auto toolSub = win->toolSub.get();
	win->render->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	arrowSize = toolSub->slider->val;
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
		win->render->DrawGeometry(path.Get(), brush.Get(),win->dpi);
	}
}

void ShapeArrow::paintDragger()
{
	for (auto& dragger : draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeArrow::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	if (hoverDraggerIndex == 0) {
		if ((modifiers & MK_SHIFT) != 0) {
			constrainToEightDirections(endX, endY, x, y, startX, startY);
		}
		else {
			startX = x;
			startY = y;
		}
		makeArrow();
	}
	else if (hoverDraggerIndex == 1) {
		if ((modifiers & MK_SHIFT) != 0) {
			constrainToEightDirections(startX, startY, x, y, endX, endY);
		}
		else {
			endX = x;
			endY = y;
		}
		makeArrow();
	}
	else if (hoverDraggerIndex == 8) {
		auto spanX{ x - pressX };
		auto spanY{ y - pressY };
		startX += spanX;
		startY += spanY;
		endX += spanX;
		endY += spanY;
		makeArrow();
		pressX = x;
		pressY = y;
	}
}

void ShapeArrow::mouseDown(const float& x, const float& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		startX = x;
		startY = y;
		hoverDraggerIndex = 1;
	}
	else if (hoverDraggerIndex == 8) {
		pressX = x;
		pressY = y;
	}
}

void ShapeArrow::mouseUp(const float& x, const float& y)
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

void ShapeArrow::mouseMove(const float& x, const float& y)
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
		BOOL contains = FALSE;
		path->FillContainsPoint({ x, y }, nullptr, &contains);
		if (contains) hoverDraggerIndex = 8;
	}
}

void ShapeArrow::setCursor()
{
	win->setCursor(IDC_SIZEALL);
}

void ShapeArrow::makeArrow()
{
	path.Reset();
	App::getD2D()->CreatePathGeometry(path.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	float dx = endX - startX;
	float dy = endY - startY;
	float length = sqrtf(dx * dx + dy * dy);
	if (length < 1.f)
	{
		sink->BeginFigure( { startX, startY }, D2D1_FIGURE_BEGIN_FILLED );
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		sink->Close();
		return;
	}
	float ux = dx / length;
	float uy = dy / length;
	float vx = -uy;
	float vy = ux;
	float v1 = arrowSize / 4.0f;        // 箭杆半宽
	float v2 = arrowSize * 2.0f / 3.0f; // 箭头半宽
	sink->BeginFigure( { startX, startY }, D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine({ endX - arrowSize * ux - v1 * vx, endY - arrowSize * uy - v1 * vy });
	sink->AddLine({ endX - (arrowSize + v1) * ux - v2 * vx, endY - (arrowSize + v1) * uy - v2 * vy });
	sink->AddLine({ endX, endY });
	sink->AddLine({ endX - (arrowSize + v1) * ux + v2 * vx, endY - (arrowSize + v1) * uy + v2 * vy });
	sink->AddLine({ endX - arrowSize * ux + v1 * vx, endY - arrowSize * uy + v1 * vy });
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
}

void ShapeArrow::constrainToEightDirections(const float& anchorX, const float& anchorY, const float& mouseX, const float& mouseY, float& targetX, float& targetY)
{
	float dx = mouseX - anchorX;
	float dy = mouseY - anchorY;
	float absX = fabsf(dx);
	float absY = fabsf(dy);
	if (absY <= absX * 0.41421356237f) {
		targetX = anchorX + dx;
		targetY = anchorY;
	}
	else if (absX <= absY * 0.41421356237f) {
		targetX = anchorX;
		targetY = anchorY + dy;
	}
	else {
		float span = absX > absY ? absX : absY;
		targetX = anchorX + (dx >= 0.f ? span : -span);
		targetY = anchorY + (dy >= 0.f ? span : -span);
	}
}
