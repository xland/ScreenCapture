#pragma once

#include <QGraphicsPathItem>
#include "../State.h"

class ShapeRect;
class ShapeDragger  : public QGraphicsPathItem
{
public:
	ShapeDragger();
	~ShapeDragger(); 
    void ShowRectDragger(ShapeRect* shapeRect);
    void hoverMove(QGraphicsSceneHoverEvent* event);
    void mouseMove(QGraphicsSceneMouseEvent* event);
    void mousePress(QGraphicsSceneMouseEvent* event);
    void mouseRelease(QGraphicsSceneMouseEvent* event);
protected:
private:
    bool hoverRectDragger(QGraphicsSceneHoverEvent* event);
private:
	QPainterPath p;
    qreal size{ 8.0f };
    std::vector<QRectF> rects;
    State state;
};
