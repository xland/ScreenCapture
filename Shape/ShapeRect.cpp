#include <qpainter.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeRect.h"
#include "../App.h"
#include "../WinBoard.h"
#include "../Tool/ToolSub.h"
#include "../WinBoard.h"
#include "../WinCanvas.h"
#include "../WinFull.h"
#include "ShapeDragger.h"

ShapeRect::ShapeRect(QObject* parent) : ShapeBase(parent)
{
    auto full = App::getFull();
    for (size_t i = 0; i < 8; i++)
    {
        draggers.push_back(QRect());
    }
    isFill = full->toolSub->getSelectState("rectFill");
    color = full->toolSub->getColor();
    strokeWidth = full->toolSub->getStrokeWidth();
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::resetDragger()
{
    auto x{ shape.x() },y{ shape.y() },w{ shape.width() },h{ shape.height() };
    draggers[0].setRect(x - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[1].setRect(x + w / 2 - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[2].setRect(x + w - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[3].setRect(x + w - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize);
    draggers[4].setRect(x + w - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[5].setRect(x + w / 2 - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[6].setRect(x - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[7].setRect(x - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize);
}

void ShapeRect::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
    }
    else {
        painter->setPen(QPen(QBrush(color), strokeWidth));
        painter->setBrush(Qt::NoBrush);
    }
    paintShape(painter);
}
void ShapeRect::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    for (size_t i = 0; i < draggers.size(); i++)
    {
        painter->drawRect(draggers[i]);
    }
}
void ShapeRect::paintShape(QPainter* painter)
{
    //auto half{ strokeWidth / 2 };
    //painter->drawRect(shape.adjusted(-half, -half, half, half));
    painter->drawRect(shape);
}
void ShapeRect::mouseMove(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeVerCursor);
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 2;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 3;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeHorCursor);
    }
    else if (draggers[4].contains(pos)) {
        hoverDraggerIndex = 4;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[5].contains(pos)) {
        hoverDraggerIndex = 5;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeVerCursor);
    }
    else if (draggers[6].contains(pos)) {
        hoverDraggerIndex = 6;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[7].contains(pos)) {
        hoverDraggerIndex = 7;
        auto board = App::getFullBoard();
        board->setCursor(Qt::SizeHorCursor);
    }
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        App::getFullCanvas()->changeShape(this);
        event->accept();
    }
}
void ShapeRect::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        shape.setTopLeft(event->pos());
        shape.setBottomRight(event->pos());
        hoverDraggerIndex = 4;
    }
    if (hoverDraggerIndex >= 0) {
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        topLeft = shape.topLeft();
        rightBottom = shape.bottomRight();
        pressPos = event->pos();
        event->accept();
        App::getFullCanvas()->update(); //它先重绘，避免闪烁
        App::getFullBoard()->update();
    }
}
void ShapeRect::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        App::getFullBoard()->update();
        App::getFullCanvas()->changeShape(this,true);
        event->accept();
    }
}
void ShapeRect::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->pos();
    if (isFill) {
        if (shape.contains(pos)) {
            hoverDraggerIndex = 8;
            auto board = App::getFullBoard();
            board->setCursor(Qt::SizeAllCursor);
        }
    }
    else {
        auto half{ strokeWidth / 2 };
        QRectF outerRect = shape.adjusted(-half, -half, half, half);
        QRectF innerRect = shape.adjusted(half, half, -half, -half);
        if (outerRect.contains(pos) && !innerRect.contains(pos)) {
            hoverDraggerIndex = 8;
            auto board = App::getFullBoard();
            board->setCursor(Qt::SizeAllCursor);
        }
    }
}
void ShapeRect::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing1) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing2) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), pos.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing3) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing4) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(),pos.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing5) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing6) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing7) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
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
    App::getFullCanvas()->update();
    event->accept();
}
