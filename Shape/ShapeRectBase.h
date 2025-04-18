#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include "ShapeBase.h"

class ShapeRectBase:public ShapeBase
{
    Q_OBJECT
public:
    ShapeRectBase(QObject* parent = nullptr);
    virtual ~ShapeRectBase();
    virtual void paint(QPainter* painter) = 0;
    virtual void paintDragger(QPainter* painter) override;
    virtual bool mouseMove(QMouseEvent* event) override;
    virtual void mouseDrag(QMouseEvent* event) override;
    virtual bool mousePress(QMouseEvent* event) override;
    virtual void mouseRelease(QMouseEvent* event) override;
    virtual void mouseOnShape(QMouseEvent* event);
public:
    QRectF shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
protected:
    QPointF topLeft, rightBottom, pressPos;
private:
    void resetDragger();
private:
};
