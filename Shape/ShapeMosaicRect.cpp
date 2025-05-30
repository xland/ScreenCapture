﻿#include <QPainter>
#include <QWindow>

#include "ShapeMosaicRect.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"
#include "../App/Util.h"

ShapeMosaicRect::ShapeMosaicRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = true;
    createMosaicImg();
}

ShapeMosaicRect::~ShapeMosaicRect()
{
}
void ShapeMosaicRect::paintDragger(QPainter* painter)
{
    ShapeRectBase::paintDragger(painter);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern({ 3,3 });
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(shape);
}
void ShapeMosaicRect::paint(QPainter* painter)
{
    bool flag = shape.isEmpty();
    if (flag) return;
    if (state != ShapeState::ready) {
        
        auto win = (WinBase*)parent();
		auto dpr = win->devicePixelRatio();
        imgPatch = mosaicImg.copy(shape.left() * dpr,shape.top() * dpr,shape.width() * dpr,shape.height() * dpr);
    }
    painter->drawImage(shape.topLeft(), imgPatch);
}

void ShapeMosaicRect::createMosaicImg()
{
    auto win = (WinBase*)parent();
    auto winImg = win->canvas->imgBg.copy();
    {
        QPainter painter(&winImg);
        for (auto& s : win->canvas->shapes)
        {
            if (s->state != ShapeState::undo) {
                s->paint(&painter);
            }
        }
    }
    winImg.setDevicePixelRatio(1.0);
    QImage mosaicPixs = winImg.scaled(winImg.width() / mosaicRectSize, winImg.height() / mosaicRectSize,
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    mosaicImg = winImg.copy();
    QPainter painter(&mosaicImg);
    painter.setPen(Qt::NoPen);
    for (int x = 0; x < mosaicPixs.width(); x++)
    {
        auto xPos = x * mosaicRectSize;
        for (int y = 0; y < mosaicPixs.height(); y++)
        {
            auto c = mosaicPixs.pixelColor(x, y);
            painter.setBrush(c);
            QRectF mRect(xPos, y * mosaicRectSize, mosaicRectSize, mosaicRectSize);
            painter.drawRect(mRect);
        }
    }
    auto dpr = win->canvas->imgBg.devicePixelRatio();
    mosaicImg.setDevicePixelRatio(dpr);
}