#include <qpainter.h>


#include "ShapeEllipse.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

 ShapeEllipse::ShapeEllipse() : QGraphicsEllipseItem(),ShapeBase()
 {
 }

ShapeEllipse::~ShapeEllipse()
{
}

bool ShapeEllipse::contains(const QPointF& point)
{
    return false;
}

void ShapeEllipse::mousePress(QGraphicsSceneMouseEvent* event)
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

void ShapeEllipse::hoverMove(QGraphicsSceneHoverEvent* event)
{
}

void ShapeEllipse::mouseRelease(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    state = ShapeState::ready;
    scene()->addItem(new ShapeEllipse());
}

void ShapeEllipse::mouseMove(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    auto pos = event->pos();
    rectShape.setBottomRight(pos);
    setRect(rectShape);
    scene()->invalidate(rectShape.adjusted(-56, -56, 56, 56));
}
