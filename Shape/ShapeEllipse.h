#pragma once
#include <QMouseEvent>
#include "ShapeRectBase.h"

class ShapeEllipse: public ShapeRectBase
{
public:
    ShapeEllipse(QObject* parent = nullptr);
    ~ShapeEllipse(); 
    void paint(QPainter* painter) override;
private:
    void mouseOnShape(QMouseEvent* event) override;
private:
    QColor color{ Qt::red };
};
