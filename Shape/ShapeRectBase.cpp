#include <qpainter.h>

#include "ShapeRectBase.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/Box.h"
#include "../Win/Canvas.h"

ShapeRectBase::ShapeRectBase(QObject* parent) : ShapeBase(parent)
{
}

ShapeRectBase::~ShapeRectBase()
{
}

void ShapeRectBase::resetDragger()
{
    prepareDraggers(8);
    auto x{ shape.x() },y{ shape.y() },w{ shape.width() },h{ shape.height() };
    auto half{ draggerSize / 2 };
    draggers[0].setRect(x - half, y - half, draggerSize, draggerSize);
    draggers[1].setRect(x + w / 2 - half, y - half, draggerSize, draggerSize);
    draggers[2].setRect(x + w - half, y - half, draggerSize, draggerSize);
    draggers[3].setRect(x + w - half, y + h / 2 - half, draggerSize, draggerSize);
    draggers[4].setRect(x + w - half, y + h - half, draggerSize, draggerSize);
    draggers[5].setRect(x + w / 2 - half, y + h - half, draggerSize, draggerSize);
    draggers[6].setRect(x - half, y + h - half, draggerSize, draggerSize);
    draggers[7].setRect(x - half, y + h / 2 - half, draggerSize, draggerSize);
}
void ShapeRectBase::paintDragger(QPainter* painter)
{
    if (draggers.empty()) return;
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setBrush(Qt::NoBrush);
    for (int i = 0; i < draggers.size(); i++)
    {
        painter->drawRect(draggers[i]);
    }
}
void ShapeRectBase::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 2;
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 3;
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
    }
    else if (draggers[4].contains(pos)) {
        hoverDraggerIndex = 4;
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[5].contains(pos)) {
        hoverDraggerIndex = 5;
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
    }
    else if (draggers[6].contains(pos)) {
        hoverDraggerIndex = 6;
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[7].contains(pos)) {
        hoverDraggerIndex = 7;
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
    }
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        showDragger();
        event->accept();
    }
}
void ShapeRectBase::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        state = (ShapeState)((int)ShapeState::sizing0 + 4);
        pressPos = event->position();
        topLeft = pressPos;
        rightBottom = pressPos;
        paintingPrepare();
        event->accept();
    }
    else if (hoverDraggerIndex >= 0) {
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        pressPos = event->position();
        topLeft = shape.topLeft();
        rightBottom = shape.bottomRight();
        paintingStart();
        event->accept();
    }
}
void ShapeRectBase::mouseRelease(QMouseEvent* event)
{
    if (shape.isEmpty()) { //鼠标按下，没有拖拽，随即释放
        deleteLater();
        event->accept();
        return;
    }
    if (state >= ShapeState::sizing0) {
        resetDragger();
        showDragger();
        event->accept();
    }
}

void ShapeRectBase::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    auto pos = event->position();
    if (state == ShapeState::sizing0) {
        shape.setCoords(pos.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing1) {
        shape.setCoords(topLeft.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing2) {
        shape.setCoords(topLeft.x(), pos.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing3) {
        shape.setCoords(topLeft.x(), topLeft.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing4) {
        shape.setCoords(topLeft.x(), topLeft.y(),pos.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing5) {
        shape.setCoords(topLeft.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing6) {
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing7) {
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::moving) {
        auto span = pos - pressPos;
        shape.translate(span);
        pressPos = pos;
    }
    if (event->modifiers() & Qt::ShiftModifier) {
        if (shape.width() > shape.height()) {
            shape.setHeight(shape.width());
        }
        else {
            shape.setWidth(shape.height());
        }
    }
    painting();
    event->accept();
}
void ShapeRectBase::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->position();
    if (isFill) {
        if (shape.contains(pos)) {
            hoverDraggerIndex = 8;
            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        }
    }
    else {
        auto half{ strokeWidth / 2 };
        QRectF outerRect = shape.adjusted(-half, -half, half, half);
        QRectF innerRect = shape.adjusted(half, half, -half, -half);
        if (outerRect.contains(pos) && !innerRect.contains(pos)) {
            hoverDraggerIndex = 8;
            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        }
    }
}