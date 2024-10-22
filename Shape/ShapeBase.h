#pragma once

#include <QWidget>
#include <QGraphicsItem>
#include "ShapeState.h"

class ShapeBase
{

public:
    ShapeBase();
    ~ShapeBase();
    virtual void hoverMove(QGraphicsSceneHoverEvent* event) = 0;
    virtual void mouseMove(QGraphicsSceneMouseEvent* event) = 0;
    virtual void mousePress(QGraphicsSceneMouseEvent* event) = 0;
    virtual void mouseRelease(QGraphicsSceneMouseEvent* event) = 0;
public:
    ShapeState state{ShapeState::temp};
};
