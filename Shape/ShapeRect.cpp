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

ShapeRect::ShapeRect(const QPoint& pos,QObject* parent) : ShapeBase(pos,parent)
{
    auto full = App::getFull();
    connect(full->board, &WinBoard::mouseDrag, this, &ShapeRect::mouseDrag);
    connect(full->board, &WinBoard::mouseMove, this, &ShapeRect::mouseMove);
    connect(full->board, &WinBoard::mousePress, this, &ShapeRect::mousePress);
    connect(full->board, &WinBoard::mouseRelease, this, &ShapeRect::mouseRelease);
    full->canvas->shape = this;


    //isFill = full->toolSub->getSelectState("rectFill");
    //color = full->toolSub->getColor();
    //strokeWidth = full->toolSub->getStrokeWidth();
}

ShapeRect::~ShapeRect()
{
}

bool ShapeRect::contains(const QPoint& point)
{
    if (isFill) {
        return shape.contains(point);
    }
    else {
        QRectF outerRect = shape.adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
        return outerRect.contains(point) && !shape.contains(point);
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
}
void ShapeRect::mouseMove(QMouseEvent* event)
{
    //isHover = contains(event->pos());
    //if (isHover) {
    //    WinBoard::Get()->cutMask->setCursor(Qt::SizeAllCursor);
    //    event->accept();
    //}
    //else{
    //    event->ignore();
    //}
}
void ShapeRect::mousePress(QMouseEvent* event)
{
    //if (isReady) {
    //    if (isHover) {
    //        isMoving = true;
    //        posPress = event->pos();
    //    }
    //    return;
    //}
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
    event->ignore();
}
void ShapeRect::mouseRelease(QMouseEvent* event)
{
    //if (isReady) {
    //    if (isHover) {
    //        WinBoard::Get()->shapeDragger->ShowRectDragger(this);
    //    }        
    //    return;
    //}
    //isReady = true;
    //WinBoard::Get()->shapeDragger->ShowRectDragger(this);
    //WinBoard::Get()->update();
}
void ShapeRect::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        state = ShapeState::sizing4;
    }
    //if (isMoving) {
    //    auto span = posPress - shape.topLeft();
    //    auto pos = event->pos();
    //    shape.moveTo(pos - span);
    //}
    if (state == ShapeState::sizing4) {
        auto pos = event->pos();
        qreal x, y, w, h;
        if (pos.x() < startPos.x()) {
            x = pos.x();
            w = startPos.x() - x;
        }
        else {
            x = startPos.x();
            w = pos.x() - x;
        }
        if (pos.y() < startPos.y()) {
            y = pos.y();
            h = startPos.y() - y;
        }
        else {
            y = startPos.y();
            h = pos.y() - y;
        }
        shape.setRect(x, y, w, h);
    }
    App::getFull()->canvas->update();
}
