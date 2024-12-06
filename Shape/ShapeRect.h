#pragma once

#include <QPainter>
#include <QObject>
#include "ShapeRectBase.h"

class ShapeRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeRect(QObject* parent = nullptr);
    virtual ~ShapeRect();
    void paint(QPainter* painter) override;
private:
    QColor color{ Qt::red };
};
