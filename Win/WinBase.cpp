#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "Shape/ShapeBase.h"
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
    setFocusPolicy(Qt::StrongFocus);
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
