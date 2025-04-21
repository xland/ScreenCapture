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
    stroker.setWidth(strokeWidth);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    prepareDraggers(2);
}

ShapeLineBase::~ShapeLineBase()
{
}

void ShapeLineBase::paintDragger(QPainter* painter)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
}
bool ShapeLineBase::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return true;  //!isStraight || 
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
    }
    else if(pathStroker.contains(pos)){
        hoverDraggerIndex = 8;
    }
    if (hoverDraggerIndex > -1) {
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        showDragger();
        event->accept();
    }
    return false;
}
bool ShapeLineBase::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        pressPos = event->position();
        isStraight = event->modifiers() & Qt::ShiftModifier;
        state = (ShapeState)((int)ShapeState::sizing0 + 1);
        path.moveTo(pressPos);
        paintingPrepare();
        return true;
    }
    else if (hoverDraggerIndex >= 0) {
        pressPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        if (hoverDraggerIndex == 0) {
            pathStart.moveTo(pressPos);
        }
        paintingStart();
        return true;
    }
    return false;
}
bool ShapeLineBase::mouseRelease(QMouseEvent* event)
{
    if (state < ShapeState::sizing0) return false;
    if (path.isEmpty()) {
        deleteLater();
        event->accept();
        return false;
    }
    if (state == ShapeState::sizing0) {
        QPainterPath tempPath;
        auto ele = pathStart.elementAt(pathStart.elementCount() - 1);
        tempPath.moveTo(ele.x, ele.y);
        for (int i = pathStart.elementCount() - 2; i >= 0; --i)
        {
            auto ele = pathStart.elementAt(i);
            tempPath.lineTo(ele.x, ele.y);
        }
        pathStart.clear();
        for (int i = 0; i < path.elementCount(); i++)
        {
            auto ele = path.elementAt(i);
            tempPath.lineTo(ele.x, ele.y);
        }
        path = tempPath;
    }
    auto startPos = path.elementAt(0);
    auto endPos = path.elementAt(path.elementCount()-1);
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x - half, startPos.y - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x - half, endPos.y - half, draggerSize, draggerSize);
    showDragger();
    paintOnBoard();
    pathStroker = stroker.createStroke(path);
    event->accept();
    return false;
}
void ShapeLineBase::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) return;
    auto pos = event->position();
    if (isStraight) {
        if (state == ShapeState::sizing0) {
            path.setElementPositionAt(0, pos.x(), pos.y());
        }
        else if (state == ShapeState::sizing1) {
            if (path.elementCount() < 2) {
                path.lineTo(pos);
            }
            else {
                path.setElementPositionAt(1, pos.x(), pos.y());
            }            
        }
        else if (state == ShapeState::moving) {
            auto span = pos - pressPos;
            auto startPos = path.elementAt(0);
            auto endPos = path.elementAt(1);
            path.setElementPositionAt(0, startPos.x+span.x(), startPos.y+span.y());
            path.setElementPositionAt(1, endPos.x+span.x(), endPos.y+span.y());
            pressPos = pos;
        }
    }
    else {
        if (state == ShapeState::sizing0) {
            pathStart.lineTo(pos);
        }
        else if (state == ShapeState::sizing1) {
            path.lineTo(pos);
        }
        else {
            auto span = pos - pressPos;
            path.translate(span);
            pressPos = pos;
        }
    }
    painting();
    event->accept();
}
