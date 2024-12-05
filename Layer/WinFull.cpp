#include "WinFull.h"

WinFull::WinFull()
{
    initSize();
    shotScreen();
    initCanvas();
}

WinFull::~WinFull()
{
}

void WinFull::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
void WinFull::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    long long rowBytes = w * 4;
    long long dataSize = rowBytes * h;
    pix.resize(dataSize);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, &pix.front(), &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
}
void WinFull::initCanvas()
{
    SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
    canvas = SkCanvas::MakeRasterDirect(imgInfo, &pix.front(), w * sizeof(SkColor));
}
void WinFull::paint(SkCanvas* canvas)
{
    
}