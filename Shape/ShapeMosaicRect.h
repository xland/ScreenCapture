#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include "ShapeRectBase.h"

class ShapeMosaicRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeMosaicRect(QObject* parent = nullptr);
    ~ShapeMosaicRect();
    virtual void paint(QPainter* painter) override;
private:
    void createMosaicImg();
private:
    QImage mosaicImg;
    QImage imgPatch;
    int mosaicRectSize{ 18 };
};
