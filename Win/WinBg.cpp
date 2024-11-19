#include <QApplication>
#include <QPainter>
#include <format>
#include <Windowsx.h>
#include <dwmapi.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>

#include "WinBg.h"
#include "WinFull.h"

WinBg::WinBg(QWidget* parent) : QWidget(parent)
{
    initSize();
    initBgImg();
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(Qt::FramelessWindowHint); //| Qt::WindowStaysOnTopHint
    setFixedSize(w, h);
    show();
    SetWindowPos((HWND)winId(), nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
WinBg::~WinBg()
{
}
void WinBg::paintEvent(QPaintEvent* event)
{    
    QPainter painter(this);    
    painter.drawPixmap(rect(), bgImg);
}

void WinBg::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
void WinBg::initBgImg()
{
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            qreal sf = screen->devicePixelRatio();
            bgImg = screen->grabWindow(0, x / sf, y / sf, w / sf, h / sf);
        }
    }
}

void WinBg::initScreens() {
    //EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
    //    {
    //        MONITORINFO info;
    //        info.cbSize = sizeof(MONITORINFO);
    //        GetMonitorInfo(hMonitor, &info);
    //        POINT leftTop{ .x{info.rcMonitor.left},.y{info.rcMonitor.top} };
    //        POINT rightBottom{ .x{info.rcMonitor.right},.y{info.rcMonitor.bottom} };
    //        auto full = (WinFull*)lParam;
    //        ScreenToClient(full->hwnd, &leftTop);
    //        ScreenToClient(full->hwnd, &rightBottom);
    //        full->screens.push_back(QRect(leftTop.x, leftTop.y, rightBottom.x + 1, rightBottom.y + 1));
    //        return TRUE;
    //    }, (LPARAM)this);
}
