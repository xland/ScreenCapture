#pragma once

#include <QPainter>
#include <QObject>

#include "ShapeRectBase.h"

class ShapeEraserRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeEraserRect(QObject* parent = nullptr);
    ~ShapeEraserRect();
    void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
protected:
    void paintingStart() override;
    void paintOnBoard() override;
    void painting() override;
};
