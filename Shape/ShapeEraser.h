#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"

class ShapeEraser:public ShapeBase
{
    Q_OBJECT
public:
    ShapeEraser(QObject* parent = nullptr);
    ~ShapeEraser();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QColor color{ Qt::red };
    qreal strokeWidth;
    bool isRect;
    ShapeState tempState;
protected:
private:
    void resetDragger();
private:
    QPointF startPos,endPos,pressPos;
    double coeffA,coeffB,coeffC,diffVal;
    QPainterPath path;
};
