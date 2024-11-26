#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeLineBase.h"

class ShapeLine:public ShapeLineBase
{
    Q_OBJECT
public:
    ShapeLine(QObject* parent = nullptr);
    ~ShapeLine();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
protected:
private:
private:
};
