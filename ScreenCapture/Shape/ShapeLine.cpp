#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeLine.h"

ShapeLine::ShapeLine(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0)}
{
	auto toolSub = WinToolSub::get();
	auto color = toolSub->getSelectedColor();
	if (toolSub->selectIndex == 0) {
		color.a = 0.5f;
	}
	win->render->CreateSolidColorBrush(color, brush.GetAddressOf());
	strokeWidth = toolSub->sliderVal;
}

ShapeLine::~ShapeLine()
{

}

void ShapeLine::paint()
{
	win->render->DrawGeometry(path.Get(), brush.Get(), strokeWidth,Util::getRoundStrokeStyle());
}

void ShapeLine::paintDragger()
{
	for (auto& dragger:draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeLine::mouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
	auto xf = static_cast<float>(x);
	auto yf = static_cast<float>(y);
	if (hoverDraggerIndex == 0) {
		if ((modifiers & MK_SHIFT) != 0) {
			auto& p = linePoints[0];
			p.x = xf;
			p.y = yf;
		}
		else {
			linePoints.insert(linePoints.begin(), { xf,yf });
		}		
		makePath();
	}
	else if (hoverDraggerIndex == 1) {
		if ((modifiers & MK_SHIFT) != 0) {
			auto& p = linePoints[linePoints.size() - 1];
			p.x = xf;
			p.y = yf;
		}
		else {
			linePoints.push_back({ xf,yf });
		}
		makePath();
	}
	else if (hoverDraggerIndex == 8) {
		auto spanX{ xf - pressX };
		auto spanY{ yf - pressY };
		for (auto& p:linePoints)
		{
			p.x += spanX;
			p.y += spanY;
		}
		makePath();
		pressX = xf;
		pressY = yf;
	}
}

void ShapeLine::mouseDown(const int& x, const int& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		linePoints.push_back({ (float)x,(float)y }); 
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			linePoints.push_back({ (float)x,(float)y });
		}
		makePath();
		hoverDraggerIndex = 1;
	}
	else if (hoverDraggerIndex == 8) {
		pressX = (float)x;
		pressY = (float)y;
	}
}

void ShapeLine::mouseUp(const int& x, const int& y)
{
	auto half{ draggerSize / 2 };
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

void ShapeLine::mouseMove(const int& x, const int& y)
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
		hitTest({ (float)x,(float)y });
    }
}

void ShapeLine::setCursor()
{
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
}

void ShapeLine::makePath()
{
	path.Reset();
	auto d2d = Util::getD2D();
	d2d->CreatePathGeometry(path.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	sink->BeginFigure(linePoints[0], D2D1_FIGURE_BEGIN_HOLLOW);
	if (linePoints.size() > 1) {
		sink->AddLines(&linePoints[1], linePoints.size() - 1);
	}	
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();
}

float  ShapeLine::pointToSegmentDistance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b)
{
	float abx = b.x - a.x, aby = b.y - a.y;
	float apx = p.x - a.x, apy = p.y - a.y;
	float ab2 = abx * abx + aby * aby;
	float t = (ab2 == 0.0f) ? 0.0f : (apx * abx + apy * aby) / ab2;
	t = std::max(0.0f, std::min(1.0f, t));  // 限制在线段范围内
	float cx = a.x + t * abx;
	float cy = a.y + t * aby;
	float dx = p.x - cx, dy = p.y - cy;

	return std::sqrtf(dx * dx + dy * dy);
}

void ShapeLine::hitTest(const D2D1_POINT_2F& mousePos)
{
	float half = strokeWidth * 0.5f + win->dpi;
	for (size_t i = 0; i + 1 < linePoints.size(); ++i) {
		if (pointToSegmentDistance(mousePos, linePoints[i], linePoints[i + 1]) <= half) {
			hoverDraggerIndex = 8;
		}
	}
}
