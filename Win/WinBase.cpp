#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "WinBase.h"
#include "WinPin.h"
#include "../App/Util.h"

WinBase::WinBase(QWidget* parent) : QMainWindow(parent)
{
}
WinBase::~WinBase()
{
}
void WinBase::initWindow()
{
    setGeometry(x, y, w, h);
    setWindowFlag(Qt::FramelessWindowHint);
    show();
    auto hwnd = (HWND)winId();
    SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    SetFocus(hwnd);
}
void WinBase::paintEvent(QPaintEvent* event)
{
}
QImage WinBase::grab(const QRect& rect)
{
    return imgBg.copy(rect);
}
void WinBase::raise()
{
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
void WinBase::move(const int& x, const int& y)
{
    this->x = x;
    this->y = y;
    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
void WinBase::close()
{
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    deleteLater();
}

void WinBase::releaseImg()
{
    if (!imgBg.isNull()) {
        imgBg = QImage();
    }
}
void WinBase::initImg()
{
    if (imgBg.isNull()) {
        imgBg = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    }
}
