#include <qpainter.h>
#include <QTransform>

#include "ShapeArrow.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"

ShapeArrow::ShapeArrow(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBox*)parent;
    isFill = win->toolSub->getSelectState("arrowFill");
    color = win->toolSub->getColor();
    arrowSize = win->toolSub->getStrokeWidth();
}

ShapeArrow::~ShapeArrow()
{
}

void ShapeArrow::resetDragger()
{
    if (draggers.empty()) {
        draggers.push_back(QRect());
        draggers.push_back(QRect());
    }
    auto half{ draggerSize / 2 };    
    draggers[0].setRect(shape[0].x() - half, shape[0].y() - half, draggerSize, draggerSize);
    draggers[1].setRect(shape[3].x() - half, shape[3].y() - half, draggerSize, draggerSize);
}

void ShapeArrow::resetShape()
{
    QLineF line(startPos, endPos);
    qreal length = line.length();
    auto angle = line.angle();
    qreal v1{ (qreal)arrowSize / 4.f };
    qreal v2{ (qreal)arrowSize / 3.f*2.f };
    shape.clear();
    shape.append(QPointF(0, 0));
    shape.append(QPointF(length - arrowSize, -v1));
    shape.append(QPointF(length - arrowSize - v1, -v2));
    shape.append(QPointF(length, 0));
    shape.append(QPointF(length - arrowSize - v1, v2));
    shape.append(QPointF(length - arrowSize, v1));
    shape.append(QPointF(0, 0));
    QTransform transform;
    transform.translate(startPos.x(), startPos.y());
    transform.rotate(-angle);
    shape = transform.map(shape);
}

void ShapeArrow::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
    }
    else {
        painter->setPen(QPen(QBrush(color), 1));
        painter->setBrush(Qt::NoBrush);
    }
    painter->drawPolygon(shape);
}
void ShapeArrow::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
}
void ShapeArrow::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }    
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBox*)parent();
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeArrow::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->position();
        hoverDraggerIndex = 1;
    }
    if (hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        event->accept();
        auto win = (WinBox*)parent();
        win->refreshBoard();
        win->refreshCanvas(this,true);
    }
}
void ShapeArrow::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        auto win = (WinBox*)parent();
        win->refreshBoard();
        win->refreshCanvas(this,true);
        event->accept();
    }
}
void ShapeArrow::mouseOnShape(QMouseEvent* event)
{
    if (shape.containsPoint(event->pos(), Qt::WindingFill)) {
        hoverDraggerIndex = 8;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
}
void ShapeArrow::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        startPos = event->pos();
        resetShape();
    }
    else if (state == ShapeState::sizing1) {
        endPos = event->pos();
        resetShape();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        shape.translate(span);
        startPos = shape[0];
        endPos = shape[3];
        pressPos = pos;
    }
    //if (event->modifiers() & Qt::ShiftModifier) {
    //    if (shape.width() > shape.height()) {
    //        shape.setHeight(shape.width());
    //    }
    //    else {
    //        shape.setWidth(shape.height());
    //    }
    //}
    auto win = (WinBox*)parent();
    win->refreshCanvas(this, true);
    event->accept();
}
