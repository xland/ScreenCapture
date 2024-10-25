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
public:
    QRect shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    void mousePress(QMouseEvent* event);
    void mouseDrag(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
    void paintDragger(QPainter* painter);
    void hoverRectDragger(QMouseEvent* event);
    void contains(QMouseEvent* event);
private:
    QPoint topLeft, rightBottom,pressPos;
};
