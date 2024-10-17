#include <qpainter.h>

#include "StrokeCtrl.h"

StrokeCtrl::StrokeCtrl(QWidget *parent):QSlider(parent)
{
}

StrokeCtrl::~StrokeCtrl()
{
}

void StrokeCtrl::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::gray);
    auto y{ height() / 2 };
    painter.drawLine(4, y, width()-8, y);
    int x = value() / maximum() * (width() - 4);
    QRect rect(x, y-4, 8, 8);
    painter.setBrush(Qt::blue);
    painter.drawEllipse(rect);
}
