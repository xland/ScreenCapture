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
private:
    QColor color{ Qt::red };
};
