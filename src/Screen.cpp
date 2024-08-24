#include "Screen.h"
#include <Windows.h>
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
    auto win = WinMax::Get();
    HDC hScreen = GetDC(NULL); // 获取整个屏幕的设备上下文
    HDC hDC = CreateCompatibleDC(hScreen); // 创建内存设备上下文
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, win->w, win->h); // 创建一个与屏幕设备上下文兼容的位图
    DeleteObject(SelectObject(hDC, hBitmap));  // 选择该位图到内存设备上下文中
    BOOL bRet = BitBlt(hDC, 0, 0, win->h, win->h, hScreen, win->x, win->y, SRCCOPY); //将屏幕内容复制到位图
    long long dataSize = win->w * 4 * win->h;
    screenPhoto.resize(dataSize);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)win->w, 0 - (long)win->h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, win->h, &screenPhoto.front(), &info, DIB_RGB_COLORS); //获取位图的像素数据
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
