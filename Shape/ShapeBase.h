#pragma once

#include <QWidget>
#include <QGraphicsItem>
#include "ShapeState.h"

class ShapeBase
{
public:
    ShapeBase();
    virtual ~ShapeBase();
    virtual bool contains(const QPointF& point) = 0;
public:
    bool isHover{ false };
    bool isReady{ false };
    int hoverDraggerIndex{ 4 };
    bool isMoving{ false };
};
