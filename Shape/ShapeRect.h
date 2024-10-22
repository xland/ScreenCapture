#pragma once

#include <QWidget>
#include <QGraphicsRectItem>

class ShapeRect: public QGraphicsRectItem
{
public:
    ShapeRect();
    ~ShapeRect();
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
public:
protected:

private:
    QPointF posPress;
    QRectF rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
