#include "WindowMain.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"

WindowMain::WindowMain()
{
    initSize();
    shotScreen();
    initWindow();
    Show();
    initCanvas();
    Refresh();
}

WindowMain::~WindowMain()
{
}

LRESULT WindowMain::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
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

void WindowMain::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    long long dataSize = w * h * 4;
    desktopPixel = new unsigned char[dataSize];
    BITMAPINFO info = {sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0};
    GetDIBits(hDC, hBitmap, 0, h, desktopPixel, &info, DIB_RGB_COLORS);
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

void WindowMain::paint(SkCanvas *canvas)
{
    // SkImageInfo info = SkImageInfo::Make(w, h, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    SkBitmap bitmap;
    bitmap.installPixels(info, desktopPixel, w * 4);
    canvas->drawImage(bitmap.asImage(), 0, 0);
}
