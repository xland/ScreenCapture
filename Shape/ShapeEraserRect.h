#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>

#include "ShapeRectBase.h"

class ShapeEraserRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeEraserRect(QObject* parent = nullptr);
    ~ShapeEraserRect();
    void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
};
