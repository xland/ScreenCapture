#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"


ShapeEraserLine::ShapeEraserLine(QObject* parent) : ShapeLineBase(parent)
{
    auto win = (WinBase*)(parent);
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeEraserLine::~ShapeEraserLine()
{
}

void ShapeEraserLine::paint(QPainter* painter)
{
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
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


void ShapeEraserLine::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        state = ShapeState::ready;
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
    }
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        tempState = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        auto win = (WinBase*)parent();
        win->refreshCanvas(this,true);
        win->refreshBoard();
        event->accept();
    }
}
void ShapeEraserLine::mouseRelease(QMouseEvent* event)
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
            auto win = (WinBase*)parent();
            win->refreshBoard();
        }
    }
    else {
        tempState = ShapeState::ready;
        event->accept();
        auto win = (WinBase*)parent();
        win->refreshBoard();
    }
}
void ShapeEraserLine::mouseDrag(QMouseEvent* event)
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
    win->refreshBoard();
    event->accept();
}
