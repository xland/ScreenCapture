#pragma once

#include <QPainter>
#include <QObject>
#include "ShapeLineBase.h"

class ShapeEraserLine:public ShapeLineBase
{
    Q_OBJECT
public:
    ShapeEraserLine(QObject* parent = nullptr);
    ~ShapeEraserLine();
    void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
public:
protected:
    void paintingStart() override;
    void paintOnBoard() override;
    void painting() override;
};
