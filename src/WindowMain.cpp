#include <windowsx.h>
#include "WindowMain.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "AppFont.h"
#include "Recorder.h"
#include "Timer.h"
#include "PixelInfo.h"
#include "Shape/ShapeDragger.h"

WindowMain *windowMain;

WindowMain::WindowMain()
{
    AppFont::Init();
    CutMask::init();
    ToolMain::init();
    ToolSub::init();
    Recorder::init();
    ShapeDragger::init();
    Timer::init();
    PixelInfo::init();
    initSize();
    shotScreen();
    initWindow();
    initCanvas();
    state = State::mask;
}

WindowMain::~WindowMain()
{
    delete Recorder::get();
    delete ToolSub::get();
    delete ToolMain::get();
    delete CutMask::get();
    delete AppFont::Get();
    windowMain = nullptr;
}

void WindowMain::init()
{
    if (!windowMain)
    {
        windowMain = new WindowMain();
        windowMain->Show();
    }
}

WindowMain *WindowMain::get()
{
    return windowMain;
}

LRESULT WindowMain::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        IsMouseDragging = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            IsMouseDragging = true;
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
    case WM_NCDESTROY:
    {
        delete this;
        exit(0);
        return true;
    }
    case WM_KEYDOWN:
    {
        switch (wparam)
        {
        case VK_ESCAPE:
        {
            this->Close(3);
            return false;
        }
        default:
        {
            onKeyDown(wparam);
        }
        }
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
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WindowMain::onMouseDown(const int& x, const int& y)
{
    auto cmFlag = CutMask::get()->OnMouseDown(x, y);
    auto tmFlag = ToolMain::get()->OnMouseDown(x, y);
    auto tsFlag =  ToolSub::get()->OnMouseDown(x, y);
    if (cmFlag || tmFlag || tsFlag) {
        return true;
    }
    Recorder::get()->OnMouseDown(x, y);
    return false;
}
bool WindowMain::onMouseDownRight(const int& x, const int& y)
{
    Recorder::get()->OnMouseDownRight(x, y);
    return false;
}
bool WindowMain::onMouseUp(const int& x, const int& y)
{
    Recorder::get()->OnMouseUp(x, y);
    CutMask::get()->OnMouseUp(x, y);
    return false;
}
bool WindowMain::onMouseMove(const int& x, const int& y)
{
    PixelInfo::get()->OnMouseMove(x, y);
    Recorder::get()->OnMouseMove(x, y);
    CutMask::get()->OnMouseMove(x, y);
    ToolMain::get()->OnMouseMove(x, y);
    ToolSub::get()->OnMouseMove(x, y);
    return false;
}
bool WindowMain::onMouseDrag(const int& x, const int& y)
{
    Recorder::get()->OnMouseDrag(x, y);
    CutMask::get()->OnMouseDrag(x, y);
    return false;
}
bool WindowMain::onChar(const unsigned int& val)
{
    Recorder::get()->onChar(val);
    return false;
}
bool WindowMain::onKeyDown(const unsigned int& val)
{
    Recorder::get()->onKeyDown(val);
    return false;
}
bool WindowMain::onMouseWheel(const int& delta)
{
    Recorder::get()->onMouseWheel(delta);
    return false;
}
//void WindowMain::paint(SkCanvas* canvas)
//{
//    Recorder::get()->OnPaint(canvas);
//    CutMask::get()->OnPaint(canvas);
//    ToolMain::get()->OnPaint(canvas);
//    ToolSub::get()->OnPaint(canvas);
//}
//void WindowMain::paintFinish(SkCanvas* canvas)
//{
//    Recorder::get()->OnPaintFinish(canvas);
//}
void WindowMain::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    long long rowBytes = w * 4;
    long long dataSize = rowBytes * h;
    auto desktop = new unsigned char[dataSize];
    BITMAPINFO info = {sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0};
    GetDIBits(hDC, hBitmap, 0, h, desktop, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
    pixSrc = new SkPixmap(imgInfo, desktop, rowBytes);
}

void WindowMain::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
