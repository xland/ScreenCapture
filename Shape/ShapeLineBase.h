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
    virtual ~ShapeLineBase();
    virtual void paintDragger(QPainter* painter) override;
    virtual bool mouseMove(QMouseEvent* event) override;
    virtual void mouseDrag(QMouseEvent* event) override;
    virtual bool mousePress(QMouseEvent* event) override;
    virtual bool mouseRelease(QMouseEvent* event) override;
public:
    qreal strokeWidth;
protected:
    QPainterPath path;
    QPainterPath pathStart;
    QPainterPath pathStroker;  //描边路径
    QPainterPathStroker stroker; 
    QPointF pressPos;
    bool isStraight{ false };
private:
};
