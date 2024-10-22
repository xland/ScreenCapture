#include <qpainter.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect() : QGraphicsRectItem(),ShapeBase()
{
    //setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    //setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::mousePress(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
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
    if (state == ShapeState::ready) {
        return;
    }
    state = ShapeState::ready;
    scene()->addItem(new ShapeRect());
}
void ShapeRect::mouseMove(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    auto pos = event->pos();
    rectShape.setBottomRight(pos);
    setRect(rectShape);
    scene()->invalidate(rectShape.adjusted(-56, -56, 56, 56));
}
