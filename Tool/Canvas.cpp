#include <QPainter>

#include "Canvas.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setGeometry(parent->rect());
    lower();
    show();
}

Canvas::~Canvas()
{
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QBrush(QColor(22, 119, 255)), 6));
    painter.drawLine(rect().topLeft(), rect().bottomRight());
}
