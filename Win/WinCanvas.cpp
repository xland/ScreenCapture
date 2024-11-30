#include <QPainter>
#include <Windows.h>

#include "WinCanvas.h"
#include "WinBase.h"
#include "../Shape/ShapeBase.h"

WinCanvas::WinCanvas(QWidget *parent) : QWidget(parent)
{
}

WinCanvas::~WinCanvas()
{
}
void WinCanvas::initWindow()
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

void WinCanvas::refresh(ShapeBase* shape, bool forceUpdate)
{
    if (shape) {
        if (shape != curShape) {
            curShape = shape;
            update();
            return;
		}        
    }
    else {
        if (shape != curShape) {
            curShape = shape;
            timer->start();
        }
    }
    if (forceUpdate) {
        update();
    }
}

void WinCanvas::init(WinBase* winBase)
{
    this->winBase = winBase;
    initWindow();
    initTimer();
}

void WinCanvas::paintEvent(QPaintEvent* event)
{
    if (!curShape) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    if (curShape->state >= ShapeState::sizing0 && !curShape->isEraser) {
        curShape->paint(&painter);
    }
    else {
        curShape->paintDragger(&painter);
    }
}

void WinCanvas::onTimeout()
{
    update();
}

void WinCanvas::initTimer()
{
    timer = new QTimer(this);
    timer->setInterval(800);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &WinCanvas::onTimeout);
}
