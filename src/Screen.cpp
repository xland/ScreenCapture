#include "Screen.h"
#include "include/core/SkRect.h"
#include <memory>
#include "WinMax.h"

namespace {
    std::shared_ptr<Screen> screen;
}

Screen* Screen::Get()
{
    return screen.get();
}

void Screen::Prepare(WinMax* win)
{
    initPosSize();
    getScreenRects();
    shotScreen();
    win->x = screen->x;
    win->y = screen->y;
    win->w = screen->w;
    win->h = screen->h;
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::Init()
{
    screen = std::shared_ptr<Screen>(new Screen());
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
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            MONITORINFO* pInfo = reinterpret_cast<MONITORINFO*>(lParam);
            GetMonitorInfo(hMonitor, pInfo);
            screen->screenRects.push_back(pInfo->rcMonitor);
            return TRUE;
        }, reinterpret_cast<LPARAM>(&info));
}

void Screen::initPosSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
