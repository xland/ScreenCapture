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
}

ShapeRect::~ShapeRect()
{
}

bool ShapeRect::contains(const QPointF& point)
{
    if (state != ShapeState::ready) {
        return false;
    }
    if (isFill) {
        return shape.contains(point);
    }
    else {
        QRectF outerRect = shape.adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
        return outerRect.contains(point) && !shape.contains(point);
    }
}

void ShapeRect::mousePress(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    posPress = event->pos();
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
    if (contains(event->pos())) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeAllCursor);
        event->accept();
    }
    else {
        event->ignore();
    }    
}

void ShapeRect::mouseRelease(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    state = ShapeState::ready;
    auto dragger = dynamic_cast<ShapeDragger*>(scene()->items()[1]);
    dragger->ShowRectDragger(this);
    scene()->addItem(new ShapeRect());
}
void ShapeRect::mouseMove(QGraphicsSceneMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
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
    auto half{ strokeWidth / 2 };
    setRect(shape.adjusted(-half,-half,half,half));
    scene()->invalidate(shape.adjusted(-56 - half, -56 - half, 56+half, 56+half));
}
