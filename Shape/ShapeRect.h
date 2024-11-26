#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include "ShapeRectBase.h"

class ShapeRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeRect(QObject* parent = nullptr);
    virtual ~ShapeRect();

};
