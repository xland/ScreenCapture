#pragma once

#include <QWidget>

#include "ShapeState.h"

class ShapeBase  : public QWidget
{
	Q_OBJECT

public:
	ShapeBase(QWidget *parent);
	~ShapeBase();
    virtual void PressEvent(QMouseEvent* event){};
    virtual void MoveEvent(QMouseEvent* event){};
    virtual void ReleaseEvent(QMouseEvent* event){};
public:
    ShapeState state{ShapeState::temp};
};
