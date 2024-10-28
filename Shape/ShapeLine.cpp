#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"


ShapeLine::ShapeLine(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    draggers.push_back(QRect());
    draggers.push_back(QRect());
    isFill = win->toolSub->getSelectState("lineFill");
    color = win->toolSub->getColor();
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeLine::~ShapeLine()
{
}

void ShapeLine::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeLine::paint(QPainter* painter)
{
    QPen pen(color);
    pen.setWidth(strokeWidth);
    painter->setPen(pen);
    painter->drawLine(startPos, endPos);
}
void ShapeLine::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
}
void ShapeLine::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBase*)parent();
        win->setCursor(Qt::SizeAllCursor);
    }   
    if (hoverDraggerIndex == -1) {
        qreal lineLength = QLineF(startPos, endPos).length();
        qreal distance = QLineF(startPos, pos).length() + QLineF(endPos, pos).length() - lineLength;
        if (distance <= strokeWidth / 2) {
            hoverDraggerIndex = 8;
            auto win = (WinBase*)parent();
            win->setCursor(Qt::SizeAllCursor);
        }
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        event->accept();
    }
}
void ShapeLine::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->pos().toPointF();
        hoverDraggerIndex = 0;
    }
    if (hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        event->accept();
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        win->update();
    }
}
void ShapeLine::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
        event->accept();
    }
}
void ShapeLine::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        endPos = event->pos();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        startPos+=span;
        endPos+=span;
        pressPos = pos;
    }
    auto win = (WinBase*)parent();
    win->canvas->update();
    event->accept();
}
