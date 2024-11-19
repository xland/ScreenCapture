#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraser.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"


ShapeEraser::ShapeEraser(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    for (int i = 0; i < 8; i++)
    {
        draggers.push_back(QRect());
    }
    isRect = win->toolSub->getSelectState("eraserFill");
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeEraser::~ShapeEraser()
{
}

void ShapeEraser::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeEraser::paint(QPainter* painter)
{
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    painter->setPen(pen);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    if (path.isEmpty()) {
        painter->drawLine(startPos, endPos);
    }
    else {
        painter->drawPath(path);
    }
    painter->restore();

}
void ShapeEraser::paintDragger(QPainter* painter)
{
    if (path.isEmpty()) {
        painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(draggers[0]);
        painter->drawRect(draggers[1]);
    }
}
void ShapeEraser::mouseMove(QMouseEvent* event)
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
        win->canvas->changeShape(this);
        event->accept();
    }
}
void ShapeEraser::mousePress(QMouseEvent* event)
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
            tempState = ShapeState::sizing0;
        }
        state = ShapeState::ready;
    }
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        tempState = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        event->accept();
    }
}
void ShapeEraser::mouseRelease(QMouseEvent* event)
{
    if (path.isEmpty()) {
        if (tempState >= ShapeState::sizing0) {
            resetDragger();
            coeffA = startPos.y() - endPos.y();
            coeffB = endPos.x() - startPos.x();
            coeffC = startPos.x() * endPos.y() - endPos.x() * startPos.y();
            diffVal = std::sqrt(coeffA * coeffA + coeffB * coeffB);
            tempState = ShapeState::ready;
            event->accept();
        }
    }
    else {
        tempState = ShapeState::ready;
        event->accept();
    }
}
void ShapeEraser::mouseDrag(QMouseEvent* event)
{
    if (tempState == ShapeState::ready) {
        return;
    }
    if (path.elementCount() <= 0) {  //这里不能判断isEmpty
        if (tempState == ShapeState::sizing0) {
            startPos = event->pos();
        }
        if (tempState == ShapeState::sizing1) {
            endPos = event->pos();
        }
        else if (tempState == ShapeState::moving) {
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
    win->update();
    event->accept();
}
