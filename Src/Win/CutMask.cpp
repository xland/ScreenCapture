#include "pch.h"
#include <dwmapi.h>
#include <include/Ling.h>
#include "CutMask.h"
#include "../Util.h"
using namespace Microsoft::WRL;

CutMask::CutMask(Ling::WinBase* win) :win{ win }
{
	strokeWidth = 2 * win->dpi;
	paddingTop *= win->dpi;
	paddingMargin *= win->dpi;
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.46f), brushBg.GetAddressOf());
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBorder.GetAddressOf());
	initWinRect();
}

bool CutMask::highlight(POINT pos)
{
	for (auto& rect : winRect)
	{
		if (pos.x > rect.left && pos.y > rect.top && pos.x < rect.right && pos.y < rect.bottom) {
			if (maskRect.left != rect.left || maskRect.top != rect.top ||
				maskRect.right != rect.right || maskRect.bottom != rect.bottom) {
				maskRect = rect;
				makeLayout();
				win->refresh();
				return true;
			}
			break;
		}
	}
	return false;
}

void CutMask::initWinRect()
{
	winRect.clear();
	EnumWindows([](HWND hwnd, LPARAM lparam)
		{
			if (!hwnd) return TRUE;
			if (!IsWindowVisible(hwnd)) return TRUE;
			if (IsIconic(hwnd)) return TRUE;
			if (GetWindowTextLength(hwnd) < 1) return TRUE;
			RECT rect;
			DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
			if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) return TRUE;
			auto self = (CutMask*)lparam;
			auto win = self->win;
			if (rect.left < win->x) rect.left = win->x;
			if (rect.top < win->y) rect.top = win->y;
			if (rect.right > win->x + win->w) rect.right = (LONG)(win->x + win->w);
			if (rect.bottom > win->y + win->h) rect.bottom = (LONG)(win->y + win->h);
			auto x = (float)(rect.left - win->x);
			auto y = (float)(rect.top - win->y);
			auto r = (float)(rect.right - win->x);
			auto b = (float)(rect.bottom - win->y);
			self->winRect.push_back(D2D1::RectF(x, y, r, b));
			return TRUE;
		}, (LPARAM)this);
}

void CutMask::makeLayout()
{
	layout.Reset();
	auto layoutStr = std::format(L"X:{} Y:{} R:{} B:{} W:{} H:{}",
		maskRect.left, maskRect.top, maskRect.right, maskRect.bottom,
		maskRect.right - maskRect.left, maskRect.bottom - maskRect.top);
	layout = Util::makeTextLayout(layoutStr, FLT_MAX, FLT_MAX, 10 * win->dpi);
	if (!layout) return;
	DWRITE_TEXT_METRICS tm = {};
	layout->GetMetrics(&tm);
	layoutRect = D2D1::RectF(maskRect.left, maskRect.top - paddingMargin - tm.height - paddingMargin * 2, maskRect.left + tm.width + paddingMargin * 2, maskRect.top - paddingMargin);
	// 标签被顶出窗口上边时折回 maskRect 内部
	if (layoutRect.top < 0) {
		auto h = layoutRect.bottom - layoutRect.top;
		auto w = layoutRect.right - layoutRect.left;
		layoutRect.top = maskRect.top + paddingMargin / 2;
		layoutRect.bottom = layoutRect.top + h;
		layoutRect.left = maskRect.left + paddingMargin;
		layoutRect.right = layoutRect.left + w;
	}
	layout->SetMaxWidth(layoutRect.right - layoutRect.left);
	layout->SetMaxHeight(layoutRect.bottom - layoutRect.top);
}

void CutMask::startMakeRect(POINT pos)
{
	pressPos = pos;
}

void CutMask::makeRect(POINT pos)
{
	auto [left, right] = std::minmax(pressPos.x, pos.x);
	auto [top, bottom] = std::minmax(pressPos.y, pos.y);
	maskRect.left = (float)left;
	maskRect.right = (float)right;
	maskRect.top = (float)top;
	maskRect.bottom = (float)bottom;
	makeLayout();
	win->refresh();
}

bool CutMask::hasRect() const
{
	return maskRect.right > maskRect.left && maskRect.bottom > maskRect.top;
}

MaskHit CutMask::hitTest(POINT pos) const
{
	if (!hasRect()) return MaskHit::None;
	const auto& r = maskRect;
	const float px = (float)pos.x, py = (float)pos.y;
	// 选区的四条边各自延长，把整个窗口切成九块：中间那块就是选区自己，
	// 落在里面一律整体拖动；外面八块各对应一个方位，落在哪块就调哪条边/哪个角
	const int col = px < r.left ? 0 : (px > r.right ? 2 : 1);
	const int row = py < r.top ? 0 : (py > r.bottom ? 2 : 1);
	static constexpr MaskHit grid[3][3]{
		{ MaskHit::TopLeft,    MaskHit::Top,    MaskHit::TopRight },
		{ MaskHit::Left,       MaskHit::Inside, MaskHit::Right },
		{ MaskHit::BottomLeft, MaskHit::Bottom, MaskHit::BottomRight },
	};
	return grid[row][col];
}

void CutMask::startAdjust(POINT pos)
{
	adjustHit = hitTest(pos);
	adjustStartRect = maskRect;
	adjustPressPos = pos;
	// 抓边或抓角：按下这一下就把对应的边挪到光标位置。
	// 内部整体拖动不能这么干 —— 那会让选区瞬间跳到光标为中心的位置。
	if (adjustHit != MaskHit::None && adjustHit != MaskHit::Inside) {
		adjust(pos);
	}
}

void CutMask::adjust(POINT pos)
{
	if (adjustHit == MaskHit::None) return;
	auto r = adjustStartRect;
	const float px = (float)pos.x, py = (float)pos.y;
	if (adjustHit == MaskHit::Inside) {
		// 整体平移，尺寸不变，夹在宿主客户区内
		const float rw = r.right - r.left, rh = r.bottom - r.top;
		const float left = std::clamp(r.left + px - adjustPressPos.x, 0.f, win->w - rw);
		const float top = std::clamp(r.top + py - adjustPressPos.y, 0.f, win->h - rh);
		r = D2D1::RectF(left, top, left + rw, top + rh);
	}
	else {
		const float cx = std::clamp(px, 0.f, win->w);
		const float cy = std::clamp(py, 0.f, win->h);
		switch (adjustHit)
		{
		case MaskHit::Left: r.left = cx; break;
		case MaskHit::Right: r.right = cx; break;
		case MaskHit::Top: r.top = cy; break;
		case MaskHit::Bottom: r.bottom = cy; break;
		case MaskHit::TopLeft: r.left = cx; r.top = cy; break;
		case MaskHit::TopRight: r.right = cx; r.top = cy; break;
		case MaskHit::BottomRight: r.right = cx; r.bottom = cy; break;
		case MaskHit::BottomLeft: r.left = cx; r.bottom = cy; break;
		default: break;
		}
		// 拖过头（比如左边越过右边）时归一化，接下来就自然变成"在拖另一条边"
		const float l = std::min(r.left, r.right), rr = std::max(r.left, r.right);
		const float t = std::min(r.top, r.bottom), b = std::max(r.top, r.bottom);
		r = D2D1::RectF(l, t, rr, b);
		// 塌到最小尺寸以下时，把动着的那条边推回来，不动锚定的那条
		const bool moveLeft = adjustHit == MaskHit::Left || adjustHit == MaskHit::TopLeft || adjustHit == MaskHit::BottomLeft;
		const bool moveTop = adjustHit == MaskHit::Top || adjustHit == MaskHit::TopLeft || adjustHit == MaskHit::TopRight;
		if (r.right - r.left < minSize) {
			if (moveLeft) r.left = r.right - minSize;
			else r.right = r.left + minSize;
		}
		if (r.bottom - r.top < minSize) {
			if (moveTop) r.top = r.bottom - minSize;
			else r.bottom = r.top + minSize;
		}
	}
	if (r.left == maskRect.left && r.top == maskRect.top &&
		r.right == maskRect.right && r.bottom == maskRect.bottom) return;
	maskRect = r;
	makeLayout();
	win->refresh();
}

void CutMask::paint(ID2D1DeviceContext* ctx)
{
	if (!layout) return;
	ctx->FillRectangle(D2D1::RectF(0.f, 0.f, win->w, maskRect.top), brushBg.Get());
	ctx->FillRectangle(D2D1::RectF(0.f, maskRect.bottom, win->w, win->h), brushBg.Get());
	ctx->FillRectangle(D2D1::RectF(0.f, maskRect.top, maskRect.left, maskRect.bottom), brushBg.Get());
	ctx->FillRectangle(D2D1::RectF(maskRect.right, maskRect.top, win->w, maskRect.bottom), brushBg.Get());
	auto halfStrokeWidth{ strokeWidth / 2.f };
	ctx->DrawRectangle(D2D1::RectF(maskRect.left - halfStrokeWidth, maskRect.top - halfStrokeWidth, maskRect.right + halfStrokeWidth, maskRect.bottom + halfStrokeWidth), brushBorder.Get(), strokeWidth);
	if (hideLabel) return;
	ctx->FillRectangle(layoutRect, brushBg.Get());
	ctx->DrawTextLayout({ layoutRect.left+ paddingMargin, layoutRect.top+ paddingMargin }, layout.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
}
