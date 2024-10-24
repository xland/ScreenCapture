#pragma once

#include <QWidget>
#include <QGraphicsItem>
#include "ShapeState.h"

class ShapeBase:public QObject
{
    Q_OBJECT
public:
    ShapeBase(const QPoint& pos,QObject* parent = nullptr);
    virtual ~ShapeBase();
    virtual bool contains(const QPoint& point) = 0;
    virtual void paint(QPainter* painter) = 0;
public:
    ShapeState state{ ShapeState::temp };
    QPoint startPos;
};
