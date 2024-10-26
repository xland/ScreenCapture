#pragma once

#include <QMouseEvent>

#include "ShapeBase.h"

class ShapeRect:public ShapeBase
{
    Q_OBJECT
public:
    ShapeRect(QObject* parent = nullptr);
    ~ShapeRect();
    void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    virtual void paintShape(QPainter* painter);
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
    virtual void mouseOnShape(QMouseEvent* event);
public:
    QRect shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    void resetDragger();
private:
    QPoint topLeft, rightBottom,pressPos;
};
