#pragma once

#include <QWidget>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "ShapeBase.h"

class ShapeEllipse: public QGraphicsEllipseItem, public ShapeBase
{
public:
    ShapeEllipse();
    ~ShapeEllipse();
    bool contains(const QPointF& point) override;
    void mousePress(QGraphicsSceneMouseEvent* event);
    void hoverMove(QGraphicsSceneHoverEvent* event);
    void mouseRelease(QGraphicsSceneMouseEvent* event);
    void mouseMove(QGraphicsSceneMouseEvent* event);
public:
protected:
private:
    QPointF posPress;
    QRectF rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
