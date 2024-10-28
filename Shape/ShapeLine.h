#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"

class ShapeLine:public ShapeBase
{
    Q_OBJECT
public:
    ShapeLine(QObject* parent = nullptr);
    ~ShapeLine();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QColor color{ Qt::red };
    qreal strokeWidth;
protected:
private:
    void resetDragger();
private:
    QPointF startPos, endPos,pressPos;
    double coeffA,coeffB,coeffC,diffVal;
};
