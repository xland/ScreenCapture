#include "pch.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "ShapeRect.h"

ShapeRect::ShapeRect(WinPin* win) :ShapeBase(win), draggers{ 
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }
{
	auto toolSub = win->toolSub.get();
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	strokeWidth = toolSub->getSliderVal();
	isFill = toolSub->isRectFill;
}

ShapeRect::~ShapeRect()
{

}

void ShapeRect::paint(ID2D1DeviceContext* ctx)
{
	if (isFill) {
		ctx->FillRectangle(rect, brush.Get());
	}
	else {
		ctx->DrawRectangle(rect, brush.Get(), strokeWidth);
	}
}

void ShapeRect::paintDragger(ID2D1DeviceContext* ctx)
{
	for (auto& dragger:draggers)
	{
		ctx->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeRect::mouseDrag(const float x, const float y)
{
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
	bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	if (hoverDraggerIndex != 8&& shiftDown) {
		auto w = rect.right - rect.left;
		auto h = rect.bottom - rect.top;
		if (w > h) {
			rect.bottom = rect.top + w;
		}
		else
		{
			rect.right = rect.left + h;
		}
	}
}

void ShapeRect::mouseDown(const float x, const float y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		pressX = x;
		pressY = y;
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
}

void ShapeRect::mouseUp(const float x, const float y)
{
	auto half{ draggerSize / 2 }, w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
	draggers[0].left = rect.left - half;
	draggers[0].top = rect.top - half;
	draggers[0].right = rect.left + half;
	draggers[0].bottom = rect.top + half;

	draggers[1].left = rect.left + w / 2 - half;
	draggers[1].top = rect.top - half;
	draggers[1].right = rect.left + w / 2 + half;
	draggers[1].bottom = rect.top + half;


	draggers[2].left = rect.right - half;
	draggers[2].top = rect.top - half;
	draggers[2].right = rect.right + half;
	draggers[2].bottom = rect.top + half;


	draggers[3].left = rect.right - half;
	draggers[3].top = rect.top + h / 2 - half;
	draggers[3].right = rect.right + half;
	draggers[3].bottom = rect.top + h / 2 + half;


	draggers[4].left = rect.right - half;
	draggers[4].top = rect.bottom - half;
	draggers[4].right = rect.right + half;
	draggers[4].bottom = rect.bottom + half;


	draggers[5].left = rect.left + w / 2 - half;
	draggers[5].top = rect.bottom - half;
	draggers[5].right = rect.left + w / 2 + half;
	draggers[5].bottom = rect.bottom + half;

	draggers[6].left = rect.left - half;
	draggers[6].top = rect.bottom - half;
	draggers[6].right = rect.left + half;
	draggers[6].bottom = rect.bottom + half;

	draggers[7].left = rect.left - half;
	draggers[7].top = rect.top + h / 2 - half;
	draggers[7].right = rect.left + half;
	draggers[7].bottom = rect.top + h / 2 + half;
}

void ShapeRect::mouseMove(const float x, const float y)
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
    else if (isInRect(draggers[2], x, y))
    {
        hoverDraggerIndex = 2;
    }
    else if (isInRect(draggers[3], x, y))
    {
        hoverDraggerIndex = 3;
    }
    else if (isInRect(draggers[4], x, y))
    {
        hoverDraggerIndex = 4;
    }
    else if (isInRect(draggers[5], x, y))
    {
        hoverDraggerIndex = 5;
    }
    else if (isInRect(draggers[6], x, y))
    {
        hoverDraggerIndex = 6;
    }
    else if (isInRect(draggers[7], x, y))
    {
        hoverDraggerIndex = 7;
    }
    if (hoverDraggerIndex == -1)
    {
        // 填充矩形没有可见边框，边缘命中区用固定宽度（2 逻辑像素）即可，
        // 别跟滑块线宽走 —— 填充时滑块那套值跟这条边没有任何关系
        auto half{ isFill ? 2.f * win->dpi : strokeWidth / 2.f + win->dpi };//多个一个dpi，让范围更大点
        if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half ) 
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
        }
    }
}

// 光标停在边框或八个 dragger 上（此时 WinPin 才把滚轮事件转过来）滚滚轮 = 调线宽，
// 与序号那边用滚轮调大小是一回事。填充矩形没有边框可调，直接不管。
void ShapeRect::mouseWheel(const float x, const float y, const short delta)
{
	if (isFill) return;
	// 一格一个逻辑像素。上下限交给 ToolSub 那张滑块值域表夹，用它夹完的返回值 ——
	// 线宽与工具栏滑块因此永远是同一个数，也滚不出滑块能表达的范围
	auto next = strokeWidth + (delta < 0 ? -win->dpi : win->dpi);
	auto applied = win->toolSub->setShapeSliderVal(L"rect", next);
	if (applied == strokeWidth) return;   //已经顶到值域的头了，不用重画
	strokeWidth = applied;
	win->refresh();
}

void ShapeRect::setCursor()
{
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
}
