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
    connect(full->board, &WinBoard::mouseDrag, this, &ShapeRect::mouseDrag);
    connect(full->board, &WinBoard::mouseMove, this, &ShapeRect::mouseMove);
    connect(full->board, &WinBoard::mousePress, this, &ShapeRect::mousePress);
    connect(full->board, &WinBoard::mouseRelease, this, &ShapeRect::mouseRelease);

    //isFill = full->toolSub->getSelectState("rectFill");
    //color = full->toolSub->getColor();
    //strokeWidth = full->toolSub->getStrokeWidth();
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::contains(QMouseEvent* event)
{
    auto pos = event->pos();
    if (isFill) {
        if (shape.contains(pos)) {
            hoverDraggerIndex = 8;
            App::getFullBoard()->setCursor(Qt::SizeAllCursor);
            event->accept();
        }
    }
    else {
        QRectF outerRect = shape.adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
        if (outerRect.contains(pos) && !shape.contains(pos)) {
            hoverDraggerIndex = 8;
            App::getFullBoard()->setCursor(Qt::SizeAllCursor);
            event->accept();
        }
    }
}
void ShapeRect::hoverRectDragger(QMouseEvent* event)
{
    auto pos = event->pos();
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        App::getFullBoard()->setCursor(Qt::SizeFDiagCursor);
        event->accept();
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        App::getFullBoard()->setCursor(Qt::SizeVerCursor);
        event->accept();
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 2;
        App::getFullBoard()->setCursor(Qt::SizeBDiagCursor);
        event->accept();
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 3;
        App::getFullBoard()->setCursor(Qt::SizeHorCursor);
        event->accept();
    }
    else if (draggers[4].contains(pos)) {
        hoverDraggerIndex = 4;
        App::getFullBoard()->setCursor(Qt::SizeFDiagCursor);
        event->accept();
    }
    else if (draggers[5].contains(pos)) {
        hoverDraggerIndex = 5;
        App::getFullBoard()->setCursor(Qt::SizeVerCursor);
        event->accept();
    }
    else if (draggers[6].contains(pos)) {
        hoverDraggerIndex = 6;
        App::getFullBoard()->setCursor(Qt::SizeBDiagCursor);
        event->accept();
    }
    else if (draggers[7].contains(pos)) {
        hoverDraggerIndex = 7;
        App::getFullBoard()->setCursor(Qt::SizeHorCursor);
        event->accept();
    }
    else {
        hoverDraggerIndex = -1;
    }
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
    auto half{ strokeWidth / 2 };
    painter->drawRect(shape.adjusted(-half, -half, half, half));
    if (state == ShapeState::active) {
        paintDragger(painter);
    }
}
void ShapeRect::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    auto half{ strokeWidth / 2 };
    auto rect = shape.adjusted(-half, -half, half, half);
    auto x{ rect.x() };
    auto y{ rect.y() };
    auto w{ rect.width() };
    auto h{ rect.height() };
    draggers.clear();
    draggers.push_back(QRect(x - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x + w / 2 - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x + w - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x + w - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x + w - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x + w / 2 - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize));
    draggers.push_back(QRect(x - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize));
    for (size_t i = 0; i < draggers.size(); i++)
    {
        painter->drawRect(draggers[i]);
    }
}
void ShapeRect::mouseMove(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        contains(event);
    }
    else if (state == ShapeState::active) {
        hoverRectDragger(event);
        if (hoverDraggerIndex == -1) {
            contains(event);
        }
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
        onActived(this);
        event->accept();
    }
    //else{
    //    posPress = event->pos();
    //    hoverDraggerIndex = 4;
    //    auto toolSub = WinBoard::Get()->toolSub;
    //    isFill = toolSub->getSelectState("rectFill");
    //    color = toolSub->getColor();
    //    strokeWidth = toolSub->getStrokeWidth();
    //    if (isFill) {
    //        setBrush(QBrush(color));
    //        setPen(Qt::NoPen);
    //    }
    //    else {
    //        setPen(QPen(QBrush(color), strokeWidth));
    //        setBrush(Qt::NoBrush);
    //    }
    //}
}
void ShapeRect::mouseRelease(QMouseEvent* event)
{
    //if (isReady) {
    //    if (isHover) {
    //        WinBoard::Get()->shapeDragger->ShowRectDragger(this);
    //    }        
    //    return;
    //}
    if (state >= ShapeState::sizing0) {
        state = ShapeState::active;
        auto canvas = App::getFull()->canvas;
        canvas->update();
    }
}
void ShapeRect::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    //if (isMoving) {
    //    auto span = posPress - shape.topLeft();
    //    auto pos = event->pos();
    //    shape.moveTo(pos - span);
    //}
    // 
    
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
    App::getFull()->canvas->update();
    event->accept();
}
