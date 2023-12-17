#include <windowsx.h>
#include "WindowMain.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "AppFont.h"
#include "Recorder.h"

WindowMain *windowMain;

WindowMain::WindowMain()
{
    AppFont::Init();
    CutMask::init();
    ToolMain::init();
    ToolSub::init();
    Recorder::init();
    initSize();
    shotScreen();
    initWindow();
    initCanvas();
    state = State::mask;
}

WindowMain::~WindowMain()
{
    delete CutMask::get();
    delete ToolMain::get();
    delete ToolSub::get();
    delete AppFont::Get();
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
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseMove(x, y);
    }
    case WM_RBUTTONDOWN:
    {
        delete this;
        exit(0);
        return true;
    }
    case WM_NCDESTROY:
    {
        delete this;
        exit(0);
        return true;
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WindowMain::onMouseDown(int x, int y)
{
    CutMask::get()->OnMouseDown(x, y);
    ToolMain::get()->OnMouseDown(x, y);
    ToolSub::get()->OnMouseDown(x, y);
    Recorder::get()->OnMouseDown(x, y);
    return false;
}
bool WindowMain::onMouseUp(int x, int y)
{
    CutMask::get()->OnMouseUp(x, y);
    ToolMain::get()->OnMouseUp(x, y);
    ToolSub::get()->OnMouseUp(x, y);
    Recorder::get()->OnMouseUp(x, y);
    return false;
}
bool WindowMain::onMouseMove(int x, int y)
{
    CutMask::get()->OnMouseMove(x, y);
    ToolMain::get()->OnMouseMove(x, y);
    ToolSub::get()->OnMouseMove(x, y);
    Recorder::get()->OnMouseMove(x, y);
    return false;
}
void WindowMain::paint(SkCanvas* canvas)
{
    CutMask::get()->OnPaint(canvas);
    ToolMain::get()->OnPaint(canvas);
    ToolSub::get()->OnPaint(canvas);
    Recorder::get()->OnPaint(canvas);
}
void WindowMain::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    long long dataSize = w * h * 4;
    pixelBase = new unsigned char[dataSize];
    BITMAPINFO info = {sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0};
    GetDIBits(hDC, hBitmap, 0, h, pixelBase, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
}

void WindowMain::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
