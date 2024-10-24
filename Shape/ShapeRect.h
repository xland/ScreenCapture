#pragma once

#include <QMouseEvent>

#include "ShapeBase.h"

class ShapeRect:public ShapeBase
{
    Q_OBJECT
public:
    ShapeRect(const QPoint& pos,QObject* parent = nullptr);
    ~ShapeRect();
    bool contains(const QPoint& point) override;
    void paint(QPainter* painter) override;
    void mousePress(QMouseEvent* event);
    void mouseDrag(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
public:
    QRect shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    QPointF posPress;
};
