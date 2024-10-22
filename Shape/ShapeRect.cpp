#include <qpainter.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect() : QGraphicsRectItem()
{
    //setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    //setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::mousePress(QGraphicsSceneMouseEvent* event)
{
    rectShape.setTopLeft(event->pos());
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

void ShapeRect::hoverMove(QGraphicsSceneHoverEvent* event)
{
}

void ShapeRect::mouseRelease(QGraphicsSceneMouseEvent* event)
{
}
void ShapeRect::mouseMove(QGraphicsSceneMouseEvent* event)
{
    auto pos = event->pos();
    rectShape.setBottomRight(pos);
    setRect(rectShape);
    scene()->invalidate(rectShape.adjusted(-56, -56, 56, 56));

    //auto pos = event->pos();
    //QRectF newRect(rectShape.topLeft(), pos);
    //setRect(newRect);
    //rectShape = newRect;
    //update();
}
