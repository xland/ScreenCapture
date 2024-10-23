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
    void mousePress(QGraphicsSceneMouseEvent* event) override;
    void hoverMove(QGraphicsSceneHoverEvent* event) override;
    void mouseRelease(QGraphicsSceneMouseEvent* event) override;
    void mouseMove(QGraphicsSceneMouseEvent* event) override;
public:
protected:
private:
    QPointF posPress;
    QRectF rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
