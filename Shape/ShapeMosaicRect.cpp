#include <QPainter>
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
    auto winImg = win->imgBg.copy();
    {
        QPainter painter(&winImg);
        for (auto& s : win->canvas->shapes)
        {
            s->paint(&painter);
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
    auto dpr = win->imgBg.devicePixelRatio();
    mosaicImg.setDevicePixelRatio(dpr);
}