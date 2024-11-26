#pragma once

#include <QObject>
#include <QMouseEvent>
#include <QPainter>
#include "ShapeState.h"

class ShapeBase:public QObject
{
    Q_OBJECT
public:
    ShapeBase(QObject* parent = nullptr);
    virtual ~ShapeBase();
    virtual void paint(QPainter* painter) = 0;
    virtual void paintDragger(QPainter* painter) = 0;
    virtual void mouseMove(QMouseEvent* event) = 0;
    virtual void mouseDrag(QMouseEvent* event) = 0;
    virtual void mousePress(QMouseEvent* event) = 0;
    virtual void mouseRelease(QMouseEvent* event) = 0;
public:
    int draggerSize{ 8 };
    ShapeState state{ ShapeState::temp };
    int hoverDraggerIndex{ -1 };
    std::vector<QRect> draggers;
    bool isEraser{ false };
};
