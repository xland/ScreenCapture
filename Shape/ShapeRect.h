#pragma once

#include <QWidget>
#include <QGraphicsRectItem>

#include "ShapeBase.h"

class ShapeRect: public QGraphicsRectItem,public ShapeBase
{
public:
    ShapeRect();
    ~ShapeRect();
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
