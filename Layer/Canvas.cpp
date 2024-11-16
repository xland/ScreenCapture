#include <QPainter>
#include "Canvas.h"
#include "../Shape/ShapeBase.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timer->setInterval(800);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &Canvas::onTimeout);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setGeometry(parent->rect());
    lower();
    show();
}

Canvas::~Canvas()
{
}

void Canvas::changeShape(ShapeBase* shape, bool forceUpdate)
{
    if (shape) {
        if (shape != curShape) {
            update();
            curShape = shape;
		}        
    }
    else {
        if (shape != curShape) {
            timer->start();
            curShape = shape;
        }
    }    
    if (forceUpdate) {
        update();
    }
}

void Canvas::paintEvent(QPaintEvent* event)
{
    if (!curShape) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    if (curShape->state >= ShapeState::sizing0) {
        curShape->paint(&painter);
    }
    else {
        curShape->paintDragger(&painter);
    }
}

void Canvas::onTimeout()
{
    update();
}
