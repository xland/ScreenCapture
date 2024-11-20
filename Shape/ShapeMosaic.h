#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"

class WinBg;
class ShapeMosaic:public ShapeBase
{
    Q_OBJECT
public:
    ShapeMosaic(QObject* parent = nullptr);
    ~ShapeMosaic();
    static void initMosaic(WinBg* winBg);
    void initImgPatch();
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
    void resetDragger();
private:
    QPointF startPos,endPos,pressPos;
    double coeffA,coeffB,coeffC,diffVal;
    QPainterPath path;
    bool isInitMosaic{ false };
    QImage imgPatch;
};
