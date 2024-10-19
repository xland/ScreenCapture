#include <qpainter.h>
#include <QWheelEvent>
#include <qtooltip.h>

#include "StrokeCtrl.h"
#include "State.h"
#include "CanvasWidget.h"

StrokeCtrl::StrokeCtrl(QWidget *parent):QSlider(parent)
{
    setVisible(false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_Hover);
    connect(this, &QSlider::valueChanged, this, &StrokeCtrl::onValueChanged);
}

StrokeCtrl::~StrokeCtrl()
{
}

void StrokeCtrl::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::gray);
    auto y{ 10 + (height()-10) / 2 };
    painter.drawLine(4, y, width()-8, y);
    qreal x = getSliderXPos();
    QRect rect(x, y-4, 8, 8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(120,120,120));
    painter.drawEllipse(rect);
}

void StrokeCtrl::wheelEvent(QWheelEvent* event)
{
    auto y = event->angleDelta().y();
    int delta = y > 0 ? 1 : -1; 
    setValue(value() + delta); 
    event->accept(); 
}

void StrokeCtrl::mousePressEvent(QMouseEvent* event)
{
    setPosByMouse(event->pos());
    event->accept();
}

void StrokeCtrl::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        setPosByMouse(event->pos());
        event->accept();
    }
}

void StrokeCtrl::showEvent(QShowEvent* event)
{
    auto canvasWidget = CanvasWidget::Get();
    if (canvasWidget->state == State::rect) {
        setMinimum(1);
        setMaximum(12);
        setValue(6);
    }
    else {
        setMinimum(1);
        setValue(22);
        setMaximum(60);
    }
    setToolTip(QString::number(value()));
}

void StrokeCtrl::onValueChanged(int value)
{
    qreal x = getSliderXPos()-8;
    auto pos = mapToGlobal(QPoint(x, -18));
    QToolTip::showText(pos, QString::number(value), this);
    setToolTip(QString::number(value));
}

void StrokeCtrl::setPosByMouse(const QPoint& pos)
{
    auto val = ((qreal)pos.x()) / ((qreal)width() - 6.f) * (qreal)maximum();
    if (val < 0) val = 1;
    if (val > maximum()) val = maximum();
    setValue(val);
}

qreal StrokeCtrl::getSliderXPos()
{
    auto val = (qreal)value();
    auto max = (qreal)maximum();
    auto min = (qreal)minimum();
    if (val == min) {
        return 1.f;
    }
    else {
        return (val - min) / (max - min) * ((qreal)width() - 12.f) + 1.f;
    }
}
