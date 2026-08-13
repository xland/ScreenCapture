#include "pch.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "History.h"
#include "ShapeEraser.h"

using Microsoft::WRL::ComPtr;

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
	auto d2d = Ling::D2D::get();
	// 橡皮没有颜色可选，这个半透明品红只在拖拽过程中当占位提示 ——
	// 松开鼠标后 isErasing 置位，改用底图画刷把这块盖回原样
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0xF00FF0, 0.38f), brush.GetAddressOf());
	strokeWidth = toolSub->getSliderVal();
	isRect = toolSub->isEraserRect;
	// 圆头圆角描边。本项目没有 App::getRoundStrokeStyle 那样的全局缓存，
	// 就每个 shape 自己持一个 —— StrokeStyle 是不可变的轻对象，代价可以忽略
	d2d->d2dFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_ROUND, 8.f, D2D1_DASH_STYLE_SOLID, 0.f
		),
		nullptr, 0, roundStyle.GetAddressOf());
}

ShapeEraser::~ShapeEraser()
{

}

void ShapeEraser::paint(ID2D1DeviceContext* ctx)
{
	if (isRect) {
		if (isErasing) {
			initBackgroundBrush();
			if (bgBrush) {
				ctx->FillRectangle(rect, bgBrush.Get());
			}
		}
		else {
			ctx->FillRectangle(rect, brush.Get());
		}
		return;
	}
	// makePath 要等第一次 mouseDown 才建 path，这之前可能先来一次 paint
	if (!path) return;
	if (isErasing) {
		initBackgroundBrush();
		if (bgBrush) {
			ctx->DrawGeometry(path.Get(), bgBrush.Get(), strokeWidth, roundStyle.Get());
		}
	}
	else {
		ctx->DrawGeometry(path.Get(), brush.Get(), strokeWidth, roundStyle.Get());
	}
}

void ShapeEraser::paintDragger(ID2D1DeviceContext* ctx)
{
	// 矩形模式 8 个夹点，涂抹模式只有首尾 2 个
	size_t count = isRect ? draggers.size() : 2;
	for (size_t i = 0; i < count; ++i)
	{
		ctx->DrawRectangle(draggers[i], brushDragger.Get(), win->dpi);
	}
}

void ShapeEraser::mouseDrag(const float x, const float y)
{
	bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	if (isRect) {
		// 几何一变就退回占位色，等 mouseUp 再重新进入擦除态
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
		if (hoverDraggerIndex != 8 && shiftDown) {
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
	// Shift 按下时不再追加顶点，而是拖动端点 —— 效果是一条直线；松开 Shift 就是自由涂抹
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

void ShapeEraser::mouseDown(const float x, const float y)
{
	if (isRect) {
		if (hoverDraggerIndex == -1) { //首次创建
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
	if (hoverDraggerIndex == -1) { //首次创建
		linePoints.push_back({ x,y });
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			// 直线模式需要两个点，末点跟着鼠标走
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

void ShapeEraser::mouseUp(const float x, const float y)
{
	updateDraggers();
	// 退化的几何（零面积矩形、只有一个点的笔画）盖回底图什么也看不出来，
	// 保持占位色让用户看到自己画了个无效的东西
	if (isRect) {
		isErasing = rect.right > rect.left && rect.bottom > rect.top;
	}
	else {
		isErasing = path && linePoints.size() > 1;
	}
}

void ShapeEraser::mouseMove(const float x, const float y)
{
	hoverDraggerIndex = -1;
	if (isRect) {
		for (size_t i = 0; i < draggers.size(); ++i) {
			if (isInRect(draggers[i], x, y)) {
				hoverDraggerIndex = (int)i;
				return;
			}
		}
		auto half{ strokeWidth / 2.f + win->dpi };//多个一个dpi，让范围更大点
		if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
		}
		return;
	}
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

void ShapeEraser::setCursor()
{
	if (isRect) {
		if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		}
		else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		}
		else if (hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		}
		else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		}
		else if (hoverDraggerIndex == 8) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
		}
		return;
	}
	if (hoverDraggerIndex == 8) {
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
	}
}

void ShapeEraser::makePath()
{
	if (linePoints.empty()) return;
	resetEraser();
	auto d2d = Ling::D2D::get();
	// ReleaseAndGetAddressOf 而不是 GetAddressOf：后者不放旧对象，自由涂抹时每个鼠标事件漏一个几何体
	d2d->d2dFactory->CreatePathGeometry(path.ReleaseAndGetAddressOf());
	if (!path) return;
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

// 底图画刷懒创建。底图与窗口同尺寸同坐标，所以不需要像 ShapeMosaic 那样平移画刷
void ShapeEraser::initBackgroundBrush()
{
	if (bgBrush || !win->screenImg) return;
	auto bitmapBrushProps = D2D1::BitmapBrushProperties(
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	auto brushProps = D2D1::BrushProperties();
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateBitmapBrush(win->screenImg.Get(), &bitmapBrushProps, &brushProps, bgBrush.GetAddressOf());
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
	t = std::max(0.0f, std::min(1.0f, t));  // 限制在线段范围内
	float cx = a.x + t * abx;
	float cy = a.y + t * aby;
	float dx = p.x - cx, dy = p.y - cy;
	return std::sqrtf(dx * dx + dy * dy);
}

void ShapeEraser::hitTest(const D2D1_POINT_2F& mousePos)
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
