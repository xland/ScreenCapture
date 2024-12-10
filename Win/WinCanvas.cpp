#include <QPainter>
#include <Windows.h>

#include "WinCanvas.h"
#include "WinBase.h"
#include "WinFull.h"
#include "../Shape/ShapeBase.h"

WinCanvas::WinCanvas(QObject *parent) : WinBase(parent)
{
    initTimer();
    auto winFull = (WinFull*)parent;
    initSizeByWin(winFull);
    img = QImage(w, h, QImage::Format_ARGB32);

    initWindow();
    show();
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

void WinCanvas::update()
{
    if (!curShape) return;
    QPainter painter(&img);
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
