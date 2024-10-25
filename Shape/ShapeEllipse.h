#pragma once
#include <QMouseEvent>
#include "ShapeBase.h"

class ShapeEllipse: public ShapeBase
{
public:
    ShapeEllipse(const QPoint& pos, QObject* parent = nullptr);
    ~ShapeEllipse();
    bool contains(const QPoint& point);
    void paint(QPainter* painter) override;
    void mousePress(QMouseEvent* event);
    void hoverMove(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
public:
protected:
private:
    QPoint posPress;
    QRect rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
