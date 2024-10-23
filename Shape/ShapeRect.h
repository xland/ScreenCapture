#pragma once

#include <QWidget>
#include <QGraphicsRectItem>

#include "ShapeBase.h"

class ShapeRect: public QObject, public QGraphicsRectItem,public ShapeBase
{
    Q_OBJECT
public:
    ShapeRect();
    ~ShapeRect();
    bool contains(const QPointF& point) override;
public:
    void mousePress(QGraphicsSceneMouseEvent* event);
    void hoverMove(QGraphicsSceneHoverEvent* event);
    void mouseRelease(QGraphicsSceneMouseEvent* event);
    void mouseMove(QGraphicsSceneMouseEvent* event);
public:
    QRectF shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    QPointF posPress;
};
