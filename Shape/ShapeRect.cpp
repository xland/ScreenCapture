#include <qpainter.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect() : QGraphicsRectItem()
{
    rectShape.setRect(460, 520, 1200, 800);
    setRect(rectShape);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setPen(QPen(QBrush(Qt::red), strokeWidth));
    setBrush(QBrush(Qt::red));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    rectShape.setTopLeft(event->pos());
    auto toolSub = CanvasWidget::Get()->toolSub;
    isFill = toolSub->getSelectState("rectFill");
    color = toolSub->getColor();
    strokeWidth = toolSub->getStrokeWidth();
}

void ShapeRect::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    auto pos = event->pos();
    rectShape.setBottomRight(pos);
    setRect(rectShape);
}

void ShapeRect::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
}
