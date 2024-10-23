#include <qpainter.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"
#include "../CutMask.h"
#include "ShapeDragger.h"

ShapeRect::ShapeRect() : QGraphicsRectItem(),ShapeBase()
{
    //setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    //setCacheMode(QGraphicsItem::ItemCoordinateCache);
    auto cutMask = CanvasWidget::Get()->cutMask;
    connect(cutMask, &CutMask::hoverMove, this, &ShapeRect::hoverMove);
    connect(cutMask, &CutMask::mouseMove, this, &ShapeRect::mouseMove);
    connect(cutMask, &CutMask::mousePress, this, &ShapeRect::mousePress);
    connect(cutMask, &CutMask::mouseRelease, this, &ShapeRect::mouseRelease);
}

ShapeRect::~ShapeRect()
{
}

bool ShapeRect::contains(const QPointF& point)
{
    if (isFill) {
        return shape.contains(point);
    }
    else {
        QRectF outerRect = shape.adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
        return outerRect.contains(point) && !shape.contains(point);
    }
}
void ShapeRect::hoverMove(QGraphicsSceneHoverEvent* event)
{
    isHover = contains(event->pos());
    if (isHover) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeAllCursor);
        event->accept();
    }
    else{
        event->ignore();
    }
}
void ShapeRect::mousePress(QGraphicsSceneMouseEvent* event)
{
    if (isReady) {
        if (isHover) {
            isMoving = true;
            posPress = event->pos();
        }
        return;
    }
    else{
        posPress = event->pos();
        hoverDraggerIndex = 4;
        auto toolSub = CanvasWidget::Get()->toolSub;
        isFill = toolSub->getSelectState("rectFill");
        color = toolSub->getColor();
        strokeWidth = toolSub->getStrokeWidth();
        if (isFill) {
            setBrush(QBrush(color));
            setPen(Qt::NoPen);
        }
        else {
            setPen(QPen(QBrush(color), strokeWidth));
            setBrush(Qt::NoBrush);
        }
    }

}
void ShapeRect::mouseRelease(QGraphicsSceneMouseEvent* event)
{
    if (isReady) {
        if (isHover) {
            CanvasWidget::Get()->shapeDragger->ShowRectDragger(this);
        }        
        return;
    }
    isReady = true;
    CanvasWidget::Get()->shapeDragger->ShowRectDragger(this);
}
void ShapeRect::mouseMove(QGraphicsSceneMouseEvent* event)
{
    if (isReady) {

        return;
    }
    if (isMoving) {
        auto span = posPress - shape.topLeft();
        auto pos = event->pos();
        shape.moveTo(pos - span);
    }
    if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4) {
        auto pos = event->pos();
        qreal x, y, w, h;
        if (pos.x() < posPress.x()) {
            x = pos.x();
            w = posPress.x() - x;
        }
        else {
            x = posPress.x();
            w = pos.x() - x;
        }
        if (pos.y() < posPress.y()) {
            y = pos.y();
            h = posPress.y() - y;
        }
        else {
            y = posPress.y();
            h = pos.y() - y;
        }
        shape.setRect(x, y, w, h);
    }
    auto half{ strokeWidth / 2 };
    setRect(shape.adjusted(-half,-half,half,half));
    scene()->invalidate(shape.adjusted(-56 - half, -56 - half, 56+half, 56+half));
}
