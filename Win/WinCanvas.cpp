#include <QPainter>
#include <Windows.h>

#include "WinCanvas.h"
#include "WinBase.h"
#include "../Shape/ShapeBase.h"

WinCanvas::WinCanvas(QWidget *parent) : WinBaseLayer(parent)
{
    initTimer();
}

WinCanvas::~WinCanvas()
{
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
