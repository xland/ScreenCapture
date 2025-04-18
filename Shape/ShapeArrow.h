#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPolygon>
#include "ShapeBase.h"

class ShapeArrow:public ShapeBase
{
    Q_OBJECT
public:
    ShapeArrow(QObject* parent = nullptr);
    ~ShapeArrow();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    bool mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QPolygonF shape;
    bool isFill{ false };
    int arrowSize{ 22 };
    QColor color{ Qt::red };
protected:
private:
    void resetDragger();
    void resetShape();
private:
    QPointF startPos, endPos,pressPos;
};
