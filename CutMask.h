#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPathItem>

class CutMask  : public QGraphicsPathItem
{
public:
    CutMask();
    ~CutMask();
public:
    QRectF maskRect;
    Qt::CursorShape cursor;
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    void changeMaskRect(const QPointF& pos);
    void changeMousePosState(const QPointF& pos);
    void updatePath();
private:
    QRectF winRect;
    QPainterPath p;
    qreal maskStroke{ 1.5 };
    QPointF dragPosition;
    int mousePosState{ -1 };
};
