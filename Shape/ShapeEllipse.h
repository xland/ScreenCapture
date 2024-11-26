#pragma once
#include <QMouseEvent>
#include "ShapeRectBase.h"

class ShapeEllipse: public ShapeRectBase
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
