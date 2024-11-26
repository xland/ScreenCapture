#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeLineBase.h"

class ShapeEraserLine:public ShapeLineBase
{
    Q_OBJECT
public:
    ShapeEraserLine(QObject* parent = nullptr);
    ~ShapeEraserLine();
    virtual void paint(QPainter* painter) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    ShapeState tempState;

};
