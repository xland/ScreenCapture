#include "pch.h"
#include "App.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "ShapeLine.h"

using Microsoft::WRL::ComPtr;

ShapeLine::ShapeLine(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0)}
{
	auto toolSub = win->toolSub.get();
	auto color = toolSub->getSelectedColor();
	isTransparent = toolSub->isLineTransparent;
	if (isTransparent) {
		color.a = 0.5f;
	}
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateSolidColorBrush(color, brush.GetAddressOf());
	strokeWidth = toolSub->getSliderVal();
	// 圆头圆角描边。本项目没有 App::getRoundStrokeStyle 那样的全局缓存，
	// 就每个 shape 自己持一个 —— StrokeStyle 是不可变的轻对象，代价可以忽略
	d2d->d2dFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,    // 起点线帽：圆角
			D2D1_CAP_STYLE_ROUND,    // 终点线帽：圆角
			D2D1_CAP_STYLE_ROUND,    // 虚线端点（如有）
			D2D1_LINE_JOIN_ROUND,    // 线段连接处：圆角
			8.f,                     // miterLimit
			D2D1_DASH_STYLE_SOLID,
			0.f
		),
		nullptr, 0, roundStyle.GetAddressOf());
}

ShapeLine::~ShapeLine()
{

}

void ShapeLine::paint(ID2D1DeviceContext* ctx)
{
	// makePath 要等第一次 mouseDown 才建 path，这之前可能先来一次 paint
	if (!path) return;
	ctx->DrawGeometry(path.Get(), brush.Get(), strokeWidth, roundStyle.Get());
}

void ShapeLine::paintDragger(ID2D1DeviceContext* ctx)
{
	for (auto& dragger:draggers)
	{
		ctx->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeLine::mouseDrag(const float x, const float y)
{
	// Shift 按下时不再追加顶点，而是拖动端点 —— 效果是一条直线；松开 Shift 就是自由画
	bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	if (hoverDraggerIndex == 0) {
		if (shiftDown) {
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
		if (shiftDown) {
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
		for (auto& p:linePoints)
		{
			p.x += spanX;
			p.y += spanY;
		}
		makePath();
		pressX = x;
		pressY = y;
	}
}

void ShapeLine::mouseDown(const float x, const float y)
{
	if (hoverDraggerIndex == -1) { //首次创建
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

void ShapeLine::mouseUp(const float x, const float y)
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

void ShapeLine::mouseMove(const float x, const float y)
{
    hoverDraggerIndex = -1;
    if (isInRect(draggers[0], x, y))
    {
        hoverDraggerIndex = 0;
    }
    else if (isInRect(draggers[1], x, y))
    {
        hoverDraggerIndex = 1;
    }
    if (hoverDraggerIndex == -1)
    {
		hitTest({ x,y });
    }
}

void ShapeLine::setCursor()
{
	if (hoverDraggerIndex == 8) {
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
	}
}

void ShapeLine::makePath()
{
	auto d2d = Ling::D2D::get();
	// ReleaseAndGetAddressOf 而不是 GetAddressOf：后者不放旧对象，自由画时每个鼠标事件漏一个几何体
	d2d->d2dFactory->CreatePathGeometry(path.ReleaseAndGetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	sink->BeginFigure(linePoints[0], D2D1_FIGURE_BEGIN_HOLLOW);
	if (linePoints.size() > 1) {
		sink->AddLines(&linePoints[1], (UINT32)(linePoints.size() - 1));
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
	// 只有笔画边缘那一圈算命中：粗笔画（马赛克、橡皮擦尤其明显）整片都能拖的话，
	// 就没法在已有笔画上面再画一笔了 —— 鼠标一按下会变成拖动旧元素。
	// 矩形模式本来就是只有边框附近才响应，这里跟它对齐
	float outer = strokeWidth * 0.5f + win->dpi; //外沿保持原来的判定范围
	float inner = outer - draggerSize / 2; //往里让出这么宽算内部。细笔画算出来是负数，等于整条线都能拖
	// 得先在所有线段里取最小距离再判断：自交的笔画里，某一段的边缘可能正好压在另一段的
	// 内部，那种位置视觉上是在笔画里面，逐段判断会误判成边缘
	float minDist{ FLT_MAX };
	for (size_t i = 0; i + 1 < linePoints.size(); ++i) {
		minDist = std::min(minDist, pointToSegmentDistance(mousePos, linePoints[i], linePoints[i + 1]));
	}
	if (minDist <= outer && minDist >= inner) {
		hoverDraggerIndex = 8;
	}
}
