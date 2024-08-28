#include "Screen.h"
#include "include/core/SkRect.h"
#include <memory>
#include "WinMax.h"

void Screen::Init()
{
    initPosSize();
    getScreenRects();
    shotScreen();
}

float Screen::GetScreenLeftByPos(const float& x, const float& y)
{
    for (size_t i = 0; i < screenRects.size(); i++)
    {
        if (screenRects[i].left <= x && 
            screenRects[i].right >= x &&
            screenRects[i].top <= y && 
            screenRects[i].bottom >= y) {
            return screenRects[i].left;
        }
    }
    return std::numeric_limits<float>::quiet_NaN();
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::shotScreen()
{
    HDC hScreen = GetDC(NULL); // 获取整个屏幕的设备上下文
    HDC hDC = CreateCompatibleDC(hScreen); // 创建内存设备上下文
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // 创建一个与屏幕设备上下文兼容的位图
    DeleteObject(SelectObject(hDC, hBitmap));  // 选择该位图到内存设备上下文中
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY); //将屏幕内容复制到位图
    long long dataSize = w * h;
    screenPix.resize(dataSize);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize*4, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, &screenPix.front(), &info, DIB_RGB_COLORS); //获取位图的像素数据
    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
}

void Screen::getScreenRects()
{
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            Screen* self = reinterpret_cast<Screen*>(lParam);
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &info);
            self->screenRects.push_back(info.rcMonitor);
            return TRUE;
        }, reinterpret_cast<LPARAM>(this));
}

void Screen::initPosSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
