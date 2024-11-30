#include <QPainter>
#include <Windows.h>

#include "WinBoard.h"
#include "WinBase.h"
#include "../Shape/ShapeBase.h"

WinBoard::WinBoard(QWidget *parent) : QWidget(parent)
{
}

WinBoard::~WinBoard()
{
}
void WinBoard::initWindow()
{
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(winBase->w, winBase->h);
    show();
    auto hwnd = (HWND)winId();
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowPos(hwnd, nullptr, winBase->x, winBase->y, winBase->w, winBase->h, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void WinBoard::init(WinBase* winBase)
{
    this->winBase = winBase;
    initWindow();
}

void WinBoard::refresh()
{
    if (refreshFlag) return;
    refreshFlag = true;
    update();
}

void WinBoard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : winBase->shapes)
    {
        if (shape->isEraser || shape->state == ShapeState::ready) {
            shape->paint(&painter);
        }
    }
    refreshFlag = false;
}
