#include "pch.h"
#include "App.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Tool/ToolSubSlider.h"
#include "History.h"
#include "Util.h"
#include "ShapeEraser.h"

ShapeEraser::ShapeEraser(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0)
}
{
	auto toolSub = win->toolSub.get();
	win->render->CreateSolidColorBrush(D2D1::ColorF(0xF00FF0, 0.38f), brush.GetAddressOf());
	strokeWidth = toolSub->slider->val;
	isRect = toolSub->selectIndex == 0;
}

ShapeEraser::~ShapeEraser()
{

}

void ShapeEraser::paint()
{
	if (isRect) {
		if (isErasing) {
			initBackgroundBrush();
			if (bgBrush.Get()) {
				win->render->FillRectangle(rect, bgBrush.Get());
			}
		}
		else {
			win->render->FillRectangle(rect, brush.Get());
		}
		return;
	}
	if (!path.Get()) return;
	if (isErasing) {
		initBackgroundBrush();
		if (bgBrush.Get()) {
			win->render->DrawGeometry(path.Get(), bgBrush.Get(), strokeWidth, App::getRoundStrokeStyle());
		}
	}
	else {
		win->render->DrawGeometry(path.Get(), brush.Get(), strokeWidth, App::getRoundStrokeStyle());
	}
}

void ShapeEraser::paintDragger()
{
	size_t count = isRect ? draggers.size() : 2;
	for (size_t i = 0; i < count; ++i)
	{
		win->render->DrawRectangle(draggers[i], brushDragger.Get(), win->dpi);
	}
}

void ShapeEraser::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	if (isRect) {
		resetEraser();
		if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4 || hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
			auto [left, right] = std::minmax(pressX, x);
			auto [top, bottom] = std::minmax(pressY, y);
			rect.left = left;
			rect.right = right;
			rect.top = top;
			rect.bottom = bottom;
		}
		else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
			auto [top, bottom] = std::minmax(pressY, y);
			rect.top = top;
			rect.bottom = bottom;
		}
		else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
			auto [left, right] = std::minmax(pressX, x);
			rect.left = left;
			rect.right = right;
		}
		else if (hoverDraggerIndex == 8) {
			auto w = rect.right - rect.left;
			auto h = rect.bottom - rect.top;
			rect.left = x - pressX;
			rect.top = y - pressY;
			rect.right = rect.left + w;
			rect.bottom = rect.top + h;
		}
		if (hoverDraggerIndex != 8 && (modifiers & MK_SHIFT) != 0) {
			auto w = rect.right - rect.left;
			auto h = rect.bottom - rect.top;
			if (w > h) {
				rect.bottom = rect.top + w;
			}
			else {
				rect.right = rect.left + h;
			}
		}
		return;
	}
	if (hoverDraggerIndex == 0) {
		if ((modifiers & MK_SHIFT) != 0) {
			auto& p = linePoints[0];
			p.x = x;
			p.y = y;
		}
		else {
			linePoints.insert(linePoints.begin(), { x,y });
		}
		makePath();
	}
	else if (hoverDraggerIndex == 1) {
		if ((modifiers & MK_SHIFT) != 0) {
			auto& p = linePoints[linePoints.size() - 1];
			p.x = x;
			p.y = y;
		}
		else {
			linePoints.push_back({ x,y });
		}
		makePath();
	}
	else if (hoverDraggerIndex == 8) {
		auto spanX{ x - pressX };
		auto spanY{ y - pressY };
		for (auto& p : linePoints)
		{
			p.x += spanX;
			p.y += spanY;
		}
		makePath();
		pressX = x;
		pressY = y;
	}
}

void ShapeEraser::mouseDown(const float& x, const float& y)
{
	if (isRect) {
		if (hoverDraggerIndex == -1) {
			pressX = x;
			pressY = y;
			rect = D2D1::RectF(x, y, x, y);
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
			pressX = rect.right;
			pressY = rect.top;
		}
		else if (hoverDraggerIndex == 7) {
			pressX = rect.right;
		}
		else if (hoverDraggerIndex == 8) {
			pressX = x - rect.left;
			pressY = y - rect.top;
		}
		return;
	}
	if (hoverDraggerIndex == -1) {
		linePoints.push_back({ x,y });
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			linePoints.push_back({ x,y });
		}
		makePath();
		hoverDraggerIndex = 1;
		win->refresh();
	}
	else if (hoverDraggerIndex == 8) {
		pressX = x;
		pressY = y;
	}
}

void ShapeEraser::mouseUp(const float& x, const float& y)
{
	updateDraggers();
	if (isRect) {
		isErasing = rect.right > rect.left && rect.bottom > rect.top;
	}
	else {
		isErasing = path.Get() && linePoints.size() > 1;
	}
}

void ShapeEraser::mouseMove(const float& x, const float& y)
{
	hoverDraggerIndex = -1;
	if (isRect) {
		for (size_t i = 0; i < draggers.size(); ++i) {
			if (Util::isInRect(draggers[i], x, y)) {
				hoverDraggerIndex = (int)i;
				return;
			}
		}
		auto half{ strokeWidth / 2.f + win->dpi };
		if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
		}
		return;
	}
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
		hitTest({ x,y });
	}
}

void ShapeEraser::setCursor()
{
	if (isRect) {
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
		return;
	}
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
}

void ShapeEraser::makePath()
{
	resetEraser();
	path.Reset();
	App::getD2D()->CreatePathGeometry(path.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	sink->BeginFigure(linePoints[0], D2D1_FIGURE_BEGIN_HOLLOW);
	if (linePoints.size() > 1) {
		sink->AddLines(&linePoints[1], (UINT32)(linePoints.size() - 1));
	}
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();
}

void ShapeEraser::resetEraser()
{
	isErasing = false;
	bgBrush.Reset();
}

void ShapeEraser::initBackgroundBrush()
{
	if (bgBrush.Get() || !win->screenImg.Get()) return;
	auto bitmapBrushProps = D2D1::BitmapBrushProperties(
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	auto brushProps = D2D1::BrushProperties();
	win->render->CreateBitmapBrush(win->screenImg.Get(), &bitmapBrushProps, &brushProps, bgBrush.GetAddressOf());
}

void ShapeEraser::updateDraggers()
{
	auto half{ draggerSize / 2 };
	if (isRect) {
		auto w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
		draggers[0] = D2D1::RectF(rect.left - half, rect.top - half, rect.left + half, rect.top + half);
		draggers[1] = D2D1::RectF(rect.left + w / 2 - half, rect.top - half, rect.left + w / 2 + half, rect.top + half);
		draggers[2] = D2D1::RectF(rect.right - half, rect.top - half, rect.right + half, rect.top + half);
		draggers[3] = D2D1::RectF(rect.right - half, rect.top + h / 2 - half, rect.right + half, rect.top + h / 2 + half);
		draggers[4] = D2D1::RectF(rect.right - half, rect.bottom - half, rect.right + half, rect.bottom + half);
		draggers[5] = D2D1::RectF(rect.left + w / 2 - half, rect.bottom - half, rect.left + w / 2 + half, rect.bottom + half);
		draggers[6] = D2D1::RectF(rect.left - half, rect.bottom - half, rect.left + half, rect.bottom + half);
		draggers[7] = D2D1::RectF(rect.left - half, rect.top + h / 2 - half, rect.left + half, rect.top + h / 2 + half);
		return;
	}
	if (linePoints.empty()) return;
	auto& start = linePoints[0];
	draggers[0].left = start.x - half;
	draggers[0].top = start.y - half;
	draggers[0].right = start.x + half;
	draggers[0].bottom = start.y + half;
	auto& end = linePoints[linePoints.size() - 1];
	draggers[1].left = end.x - half;
	draggers[1].top = end.y - half;
	draggers[1].right = end.x + half;
	draggers[1].bottom = end.y + half;
}




float  ShapeEraser::pointToSegmentDistance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b)
{
	float abx = b.x - a.x, aby = b.y - a.y;
	float apx = p.x - a.x, apy = p.y - a.y;
	float ab2 = abx * abx + aby * aby;
	float t = (ab2 == 0.0f) ? 0.0f : (apx * abx + apy * aby) / ab2;
	t = std::max(0.0f, std::min(1.0f, t));
	float cx = a.x + t * abx;
	float cy = a.y + t * aby;
	float dx = p.x - cx, dy = p.y - cy;
	return std::sqrtf(dx * dx + dy * dy);
}

void ShapeEraser::hitTest(const D2D1_POINT_2F& mousePos)
{
	float half = strokeWidth * 0.5f + win->dpi;
	for (size_t i = 0; i + 1 < linePoints.size(); ++i) {
		if (pointToSegmentDistance(mousePos, linePoints[i], linePoints[i + 1]) <= half) {
			hoverDraggerIndex = 8;
		}
	}
}
