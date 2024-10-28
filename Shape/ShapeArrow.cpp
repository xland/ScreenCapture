#include <qpainter.h>

#include "ShapeArrow.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"

ShapeArrow::ShapeArrow(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    draggers.push_back(QRect());
    draggers.push_back(QRect());
    isFill = win->toolSub->getSelectState("rectFill");
    color = win->toolSub->getColor();
    arrowSize = win->toolSub->getStrokeWidth();
}

ShapeArrow::~ShapeArrow()
{
}

void ShapeArrow::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
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
    QLineF line(startPos, endPos);
    qreal angle = line.angle();
    qreal length = line.length();
    auto half{ draggerSize / 2 };
    painter->save();
    painter->translate(startPos);
    painter->rotate(angle);
    shape.append(QPoint(0, -half));
    shape.append(QPoint(length-arrowSize, -half));
    shape.append(QPoint(length - arrowSize, -arrowSize));
    shape.append(QPoint(length, 0));
    shape.append(QPoint(length - arrowSize, arrowSize));
    shape.append(QPoint(length - arrowSize, half));
    shape.append(QPoint(0, half));
    shape.end();
    painter->drawPolygon(shape);
    painter->restore();
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
        auto win = (WinBase*)parent();
        win->setCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->setCursor(Qt::SizeAllCursor);
    }    
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        event->accept();
    }
}
void ShapeArrow::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->pos();
        hoverDraggerIndex = 1;
    }
    if (hoverDraggerIndex >= 0) {
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        event->accept();
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        win->update();
    }
}
void ShapeArrow::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
        event->accept();
    }
}
void ShapeArrow::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->pos();
    if (shape.contains(pos)) {
        hoverDraggerIndex = 8;
        auto board = (WinBase*)parent();
        board->setCursor(Qt::SizeAllCursor);
    }
}
void ShapeArrow::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        startPos = event->pos();
    }
    else if (state == ShapeState::sizing1) {
        endPos = event->pos();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - startPos;
        shape.translate(span);
        startPos = pos;
    }
    //if (event->modifiers() & Qt::ShiftModifier) {
    //    if (shape.width() > shape.height()) {
    //        shape.setHeight(shape.width());
    //    }
    //    else {
    //        shape.setWidth(shape.height());
    //    }
    //}
    auto win = (WinBase*)parent();
    win->canvas->update();
    event->accept();
}
