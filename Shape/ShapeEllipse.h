#pragma once
#include <QMouseEvent>
#include "ShapeRect.h"

class ShapeEllipse: public ShapeRect
{
public:
    ShapeEllipse(QObject* parent = nullptr);
    ~ShapeEllipse(); 
    void paint(QPainter* painter) override;
    void mouseOnShape(QMouseEvent* event) override;
public:
protected:
private:
};
