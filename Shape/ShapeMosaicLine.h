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
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QColor color{ Qt::red };
    qreal strokeWidth;
    bool isRect;
protected:
private:
    void initMosaicBgImg();
    void resetDragger();
private:
    QPointF startPos,endPos,pressPos;
    double coeffA,coeffB,coeffC,diffVal;
    QPainterPath path;
    bool isInitMosaic{ false };
    QImage imgPatch;
    QImage winImg;
    QImage mosaicImg;
    QRect pathRect;
};
