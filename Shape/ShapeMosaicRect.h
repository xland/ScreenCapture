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
    void paint(QPainter* painter) override;
	void paintDragger(QPainter* painter) override;
private:
    void createMosaicImg();
private:
    QImage mosaicImg;
    QImage imgPatch;
    int mosaicRectSize{ 18 };
};
