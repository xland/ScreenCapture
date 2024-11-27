#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"
#include "ShapeLineBase.h"

class WinBg;
class ShapeMosaicLine:public ShapeLineBase
{
    Q_OBJECT
public:
    ShapeMosaicLine(QObject* parent = nullptr);
    ~ShapeMosaicLine();
    virtual void paint(QPainter* painter) override;
    void mouseRelease(QMouseEvent* event) override;
private:
    QImage imgPatch;
    QImage* winImg;
    QImage* mosaicImg;
    QRectF pathRect;
};
