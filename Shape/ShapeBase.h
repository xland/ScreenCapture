#pragma once

#include <QWidget>
#include <QGraphicsItem>
#include "ShapeState.h"

class ShapeBase  : public QGraphicsItem
{

public:
    ShapeBase();
    ~ShapeBase();
public:
    ShapeState state{ShapeState::temp};
};
