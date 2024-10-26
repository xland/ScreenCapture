#pragma once
#include <QMouseEvent>
#include "ShapeRect.h"

class ShapeEllipse: public ShapeRect
{
public:
    ShapeEllipse(QObject* parent = nullptr);
    ~ShapeEllipse();
    void paintShape(QPainter* painter) override;
    void mouseOnShape(QMouseEvent* event) override;
public:
protected:
private:
};
