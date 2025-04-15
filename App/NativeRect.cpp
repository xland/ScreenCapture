#include <Windows.h>
#include <dwmapi.h>

#include "Util.h"
#include "NativeRect.h"

namespace {
    static QList<QRect> winRects;
    static QList<QRect> screenRects;
    static QRect desktopRect;
}

void NativeRect::init()
{
    initDesktopRect();
	initWinRect();
	initScreenRect();
}

const QList<QRect>& NativeRect::getWinRects()
{
    return winRects;
}

const QList<QRect>& NativeRect::getScreenRects()
{
    return screenRects;
}

const QRect& NativeRect::getDesktopRect()
{
    return desktopRect;
}

void NativeRect::initWinRect()
{
    //winRects.clear();
    //EnumWindows([](HWND hwnd, LPARAM lparam)
    //    {
    //        if (!hwnd) return TRUE;
    //        if (!IsWindowVisible(hwnd)) return TRUE;
    //        if (IsIconic(hwnd)) return TRUE;
    //        if (GetWindowTextLength(hwnd) < 1) return TRUE;
    //        RECT rect;
    //        DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
    //        if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
    //            return TRUE;
    //        }
    //        QPoint lt(rect.left - desktopRect.x(), rect.top - desktopRect.y());
    //        QPoint rb(rect.right - desktopRect.x(), rect.bottom - desktopRect.y());
    //        winRects.push_back(QRect(lt, rb));
    //        return TRUE;
    //    },NULL);
}

void NativeRect::initScreenRect()
{
    screenRects.clear();
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &info);
            QPoint lt(info.rcMonitor.left, info.rcMonitor.top);
            QPoint br(info.rcMonitor.right, info.rcMonitor.bottom);
            screenRects.push_back(QRect(lt, br));
            return TRUE;
        }, NULL);
}

void NativeRect::initDesktopRect()
{
    auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    desktopRect = QRect(x, y, w, h);
}

const QRect* NativeRect::getScreenByPos(const int& x, const int& y)
{
    for (int i = 0; i < screenRects.size(); i++)
    {
        if (screenRects[i].contains(x, y)) {
            return &screenRects[i];
        }
    }
    return nullptr;
}
