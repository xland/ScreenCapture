#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinCap.h"
#include "WinCutMask.h"
#include "WinBase.h"

WinCutMask::WinCutMask(WinBase* win):win{win}
{
    strokeWidth = 2 * win->dpi;
    auto render = win->render.Get();
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.46f), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBorder.GetAddressOf());
	paddingTop *= win->dpi;
    initWinRect();
}

WinCutMask::~WinCutMask() {

}

bool WinCutMask::highlight(const int& x, const int& y)
{
    for (auto& rect : winRect)
    {
        if (x>rect.left&&y>rect.top&&x<rect.right&&y<rect.bottom) {
            if (maskRect.left != rect.left||maskRect.top != rect.top || 
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

void WinCutMask::initWinRect()
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
            auto self = (WinCutMask*)lparam;
            if (rect.left < self->win->x) rect.left = self->win->x;
            if (rect.top < self->win->y) rect.top = self->win->y;
            if (rect.right > self->win->x + self->win->w) rect.right = self->win->x + self->win->w;
            if (rect.bottom > self->win->y + self->win->h) rect.bottom = self->win->y + self->win->h;
            auto x = (rect.left - self->win->x);
            auto y = (rect.top - self->win->y);
            auto r = (rect.right - self->win->x);
            auto b = (rect.bottom - self->win->y);
            self->winRect.push_back(D2D1::RectF(x, y, r, b));
            return TRUE;
        }, (LPARAM)this);
}
void WinCutMask::makeLayout()
{
    layout.Reset();
    auto layoutStr = std::format(L"X:{} Y:{} R:{} B:{} W:{} H:{}",
        maskRect.left, maskRect.top, maskRect.right, maskRect.bottom,
        maskRect.right - maskRect.left, maskRect.bottom - maskRect.top);
    layout = win->makeTextLayout(layoutStr, FLT_MAX, FLT_MAX, 10 * win->dpi);
    DWRITE_TEXT_METRICS tm = {};
    layout->GetMetrics(&tm);
    float paddingLeft{ 5 * win->dpi }, margin{ 5 * win->dpi };
    layoutRect = D2D1::RectF(maskRect.left, maskRect.top - margin - tm.height - paddingTop * 2, maskRect.left + tm.width + paddingLeft * 2, maskRect.top - margin);
    if (layoutRect.top < win->y) {
        auto h = layoutRect.bottom - layoutRect.top;
        auto w = layoutRect.right - layoutRect.left;
        layoutRect.top = maskRect.top + margin / 2;
        layoutRect.bottom = layoutRect.top + h;
        layoutRect.left = maskRect.left + margin;
        layoutRect.right = layoutRect.left + w;
    }
    layout->SetMaxWidth(layoutRect.right - layoutRect.left);
    layout->SetMaxHeight(layoutRect.bottom - layoutRect.top);
}
void WinCutMask::startMakeRect(const int& x, const int& y)
{
    pressX = x;
    pressY = y;
}

void WinCutMask::makeRect(const int& x, const int& y)
{
    auto [left, right] = std::minmax(pressX, x);
    auto [top, bottom] = std::minmax(pressY, y);
    maskRect.left = left;
    maskRect.right = right;
    maskRect.top = top;
    maskRect.bottom = bottom;
    makeLayout();
    win->refresh();
}

void WinCutMask::paint()
{
	if (!layout.Get()) return;
    auto render = win->render.Get();
    render->FillRectangle(D2D1::RectF(0.f, 0.f,win->w, maskRect.top), brushBg.Get());
    render->FillRectangle(D2D1::RectF(0.f, maskRect.bottom, win->w, win->h), brushBg.Get());
    render->FillRectangle(D2D1::RectF(0.f, maskRect.top, maskRect.left, maskRect.bottom), brushBg.Get());
    render->FillRectangle(D2D1::RectF(maskRect.right, maskRect.top, win->w, maskRect.bottom), brushBg.Get());
    auto halfStrokeWidth{ strokeWidth / 2.f };
    render->DrawRectangle(D2D1::RectF(maskRect.left - halfStrokeWidth, maskRect.top - halfStrokeWidth, maskRect.right + halfStrokeWidth, maskRect.bottom + halfStrokeWidth), brushBorder.Get(), strokeWidth);
    render->FillRoundedRectangle(D2D1::RoundedRect(layoutRect, paddingTop, paddingTop), brushBg.Get());
    render->DrawTextLayout({ layoutRect.left, layoutRect.top }, layout.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);


    //通过这种方式可以擦掉指定的区域
    //render->PushAxisAlignedClip(cutMask->maskRect, D2D1_ANTIALIAS_MODE_ALIASED);
    //render->Clear(D2D1::ColorF(0, 0, 0, 0));   // Alpha=0 即“擦掉”
    //render->PopAxisAlignedClip();
}
