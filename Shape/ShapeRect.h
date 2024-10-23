#pragma once

#include <QWidget>
#include <QGraphicsRectItem>

#include "ShapeBase.h"

class ShapeRect: public QGraphicsRectItem,public ShapeBase
{
public:
    ShapeRect();
    ~ShapeRect();
    bool contains(const QPointF& point) override;
    void mousePress(QGraphicsSceneMouseEvent* event) override;
    void hoverMove(QGraphicsSceneHoverEvent* event) override;
    void mouseRelease(QGraphicsSceneMouseEvent* event) override;
    void mouseMove(QGraphicsSceneMouseEvent* event) override;
public:
    QRectF shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    QPointF posPress;
};
