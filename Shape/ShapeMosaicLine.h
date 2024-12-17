#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeLineBase.h"


class ShapeMosaicLine:public ShapeLineBase
{
    Q_OBJECT
public:
    ShapeMosaicLine(QObject* parent = nullptr);
    ~ShapeMosaicLine();
    void paint(QPainter* painter) override;
    void mouseRelease(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;

private:
    void erasePath(QImage* img);
    void createMosaicImg();
private:
    int mosaicRectSize{ 18 };
    QImage imgPatch;
    QImage* winImg;
    QImage* mosaicImg;
    QRectF pathRect;
};
