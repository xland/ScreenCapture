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
    void mouseRelease(QMouseEvent* event) override;
private:
    void paintMosicRects(QPainter* painter);
private:
    QImage imgPatch;
    QImage mosaicPixs;
    int mosaicRectSize{ 18 };
};
