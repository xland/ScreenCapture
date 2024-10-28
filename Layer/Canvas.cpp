#include <QPainter>
#include "Canvas.h"
#include "../Shape/ShapeBase.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
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
        timer->start(800);
    }
    if (shape != curShape) {
        curShape = shape;
        if (shape) {
            update();
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
