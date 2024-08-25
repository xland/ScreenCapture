#include "WinMax.h"
#include <memory>
#include <Windows.h>
#include <include/core/SkImage.h>
#include <include/core/SkPixmap.h>
#include <include/core/SkBitmap.h>
#include "Screen.h"


namespace {
	std::shared_ptr<WinMax> winMax;
}

WinMax::WinMax()
{
    auto screen = Screen::Get();
    screen->Prepare(this);
    initSurface();
    initWindow();
}

WinMax::~WinMax()
{
}

WinMax* WinMax::Get()
{
    return winMax.get();
}

void WinMax::Init()
{
	auto ptr = new WinMax();
	winMax = std::shared_ptr<WinMax>(ptr);
}

LRESULT WinMax::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        IsMouseDown = true;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseDown(x, y);
    }
    case WM_LBUTTONDBLCLK:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onDoubleClick(x, y);
    }
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            return onMouseDrag(x, y);
        }
        else {
            return onMouseMove(x, y);
        }
    }
    case WM_RBUTTONDOWN:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        onMouseDownRight(x, y);
        return true;
    }
    case WM_KEYDOWN:
    {
        onKeyDown(wparam);
        return false;
    }
    case WM_CHAR: {
        return onChar(wparam);
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        onMouseWheel(delta);
        return false;
    }
    case WM_TIMER: {
        return onTimeout(wparam);
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinMax::onPaint()
{
    auto screen = Screen::Get();
    auto rowSize = w * sizeof(SkColor);
    auto info = winCanvas->imageInfo();
    winCanvas->writePixels(info, &screen->screenPix.front(), rowSize, 0, 0);

    SkPixmap pixmap(info, &canvasPix.front(), rowSize);
    SkBitmap bitmap;
    bitmap.installPixels(pixmap);
    bitmap.setImmutable();
    auto img = bitmap.asImage();
    winCanvas->drawImage(img, 0, 0);
}
