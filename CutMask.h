#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include "State.h"

class CutMask  : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    CutMask();
    ~CutMask();
public:
    QRectF maskRect;
    bool isShapeUseMouse{ false };
signals:
    void hoverMove(QGraphicsSceneHoverEvent* e);
    void mouseMove(QGraphicsSceneMouseEvent* e);
    void mousePress(QGraphicsSceneMouseEvent* e);
    void mouseRelease(QGraphicsSceneMouseEvent* e);
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    void changeMaskRect(const QPointF& pos);
    void changeMousePosState(const qreal& x,const qreal& y);
    void changeMousePosState2(const qreal& x, const qreal& y);
    void updatePath();
    void addShape(const State& state);
private:
    QRectF winRect;
    QPainterPath p;
    qreal maskStroke{ 1.8 };
    QPointF dragPosition;
    int mousePosState{ -1 };
};
