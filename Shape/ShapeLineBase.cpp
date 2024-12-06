#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeLineBase.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"


ShapeLineBase::ShapeLineBase(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)(parent);
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeLineBase::~ShapeLineBase()
{
}

void ShapeLineBase::resetDragger()
{
    if (draggers.empty()) {
        draggers.push_back(QRect());
        draggers.push_back(QRect());
    }
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}
void ShapeLineBase::paintDragger(QPainter* painter)
{
    if (path.isEmpty() && !draggers.empty()) {
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(1);
        painter->setBrush(Qt::NoBrush);
        if (isEraser) {
            QLineF line(startPos, endPos);
            auto length = line.length();
            auto angle = line.angle();
            auto half = strokeWidth / 2;
            QPolygonF borderShape;
            borderShape.append(QPointF(-half, -half));
            borderShape.append(QPointF(length+half,-half));
            borderShape.append(QPointF(length+half, half));
            borderShape.append(QPointF(-half, half));
            borderShape.append(QPointF(-half, -half));
            QTransform transform;
            transform.translate(startPos.x(), startPos.y());
            transform.rotate(-angle);
            borderShape = transform.map(borderShape);
            pen.setStyle(Qt::CustomDashLine);
            pen.setDashPattern({ 3,3 });
            painter->setPen(pen);
            painter->drawPolygon(borderShape);
        }
        else {            
            painter->setPen(pen);
            painter->drawRect(draggers[0]);
            painter->drawRect(draggers[1]);
        }
    }
}
void ShapeLineBase::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    if (!path.isEmpty()) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    if (hoverDraggerIndex == -1) {
        double distance = std::abs(coeffA * pos.x() + coeffB * pos.y() + coeffC) / diffVal;
        if (distance <= strokeWidth / 2) {
            hoverDraggerIndex = 8;
            auto win = (WinBase*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBase*)parent();
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeLineBase::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        auto flag = event->modifiers() & Qt::ShiftModifier;
        if (flag) {
            startPos = event->pos().toPointF();
            endPos = startPos;
            hoverDraggerIndex = 1;
        }
        else {
            path.moveTo(event->pos());
            path.lineTo(event->pos());
            state = ShapeState::sizing0;
            auto win = (WinBase*)parent();
            win->refreshCanvas(isEraser?nullptr:this,true);
        }
    }
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        auto win = (WinBase*)parent();
        win->refreshCanvas(isEraser ? nullptr : this, true);
        win->refreshBoard();
        event->accept();
    }
}
void ShapeLineBase::mouseRelease(QMouseEvent* event)
{
    if (path.isEmpty()) {
        if (state >= ShapeState::sizing0) {
            resetDragger();
            coeffA = startPos.y() - endPos.y();
            coeffB = endPos.x() - startPos.x();
            coeffC = startPos.x() * endPos.y() - endPos.x() * startPos.y();
            diffVal = std::sqrt(coeffA * coeffA + coeffB * coeffB);
            state = ShapeState::ready;

            auto win = (WinBase*)parent();
            win->refreshBoard();
            win->refreshCanvas(this,true);
            event->accept();
        }
    }
    else {
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->refreshBoard();
        event->accept();
    }
}
void ShapeLineBase::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (path.elementCount() <= 0) {  //这里不能判断isEmpty
        if (state == ShapeState::sizing0) {
            startPos = event->pos();
        }
        if (state == ShapeState::sizing1) {
            endPos = event->pos();
        }
        else if (state == ShapeState::moving) {
            auto pos = event->pos();
            auto span = pos - pressPos;
            startPos += span;
            endPos += span;
            pressPos = pos;
        }
    }
    else {
        path.lineTo(event->position());
    }
    auto win = (WinBase*)parent();
    if (isEraser) {
        win->refreshBoard();
    }
    else {
        win->refreshCanvas(this, true);
    }
    event->accept();
}
