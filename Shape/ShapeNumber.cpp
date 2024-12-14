#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeNumber.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"

namespace {
    static int numVal{ 0 };
}


ShapeNumber::ShapeNumber(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBox*)parent;
    prepareDraggers(1);
    isFill = win->toolSub->getSelectState("numberFill");
    color = win->toolSub->getColor();
    val = ++numVal;
}

ShapeNumber::~ShapeNumber()
{
}

void ShapeNumber::resetDragger()
{
    auto half{ draggerSize / 2 };    
    draggers[0].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeNumber::resetShape()
{
    //startPos圆心；endPos箭头顶点
    QLineF line(startPos, endPos);
    qreal length = line.length();
    auto angle = line.angle();
    r = length/4.f*3.f;
    double y = r * sin(10 * std::numbers::pi / 180.0); // 对应的直角边
    double x = r * cos(10 * std::numbers::pi / 180.0); // 另一个直角边
    shape.clear();
    shape.moveTo(x, -y);
    shape.arcTo(QRectF(-r,-r, 2 * r, 2 * r), 10, 340);
    shape.lineTo(length,0);
    shape.lineTo(x, -y);
    
    QTransform transform;
    transform.translate(startPos.x(), startPos.y());
    transform.rotate(-angle);
    shape = transform.map(shape);
    
}

void ShapeNumber::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
        painter->drawPath(shape);
        painter->setPen(QColor(255, 255, 255));
    }
    else {
        painter->setPen(QPen(QBrush(color), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(shape);
        painter->setPen(color);
    }
    QRectF rect(startPos.x() - r, startPos.y() - r, 2 * r, 2 * r);
    auto font = painter->font();
    font.setPointSizeF(r / 4 * 3);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(val));
}
void ShapeNumber::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
}
void ShapeNumber::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
    }   
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        showDragger();
        event->accept();
    }
}
void ShapeNumber::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->position();
        hoverDraggerIndex = 0;
    }
    if (hoverDraggerIndex >= 0) {
        pressPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        paintingStart();
        event->accept();
    }
}
void ShapeNumber::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        showDragger();
        event->accept();
    }
}

void ShapeNumber::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        endPos = event->pos();
        resetShape();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        shape.translate(span);
        startPos+=span;
        endPos+=span;
        pressPos = pos;
    }
    painting();
    event->accept();
}
void ShapeNumber::mouseOnShape(QMouseEvent* event)
{
    if (shape.contains(event->pos())) {
        hoverDraggerIndex = 8;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
}