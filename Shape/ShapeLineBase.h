#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"

class ShapeLineBase:public ShapeBase
{
    Q_OBJECT
public:
    ShapeLineBase(QObject* parent = nullptr);
    ~ShapeLineBase();
    virtual void paintDragger(QPainter* painter) override;
    virtual void mouseMove(QMouseEvent* event) override;
    virtual void mouseDrag(QMouseEvent* event) override;
    virtual void mousePress(QMouseEvent* event) override;
    virtual void mouseRelease(QMouseEvent* event) override;
public:
    qreal strokeWidth;
protected:
protected:
    QPainterPath path;
    QPainterPath pathStart;
    QPainterPath pathStroker;  //描边路径
    QPainterPathStroker stroker; 
    QPointF pressPos;
    QPointF startPos, endPos;
    double coeffA, coeffB, coeffC, diffVal;
    bool isStraight{ false };
private:
};
