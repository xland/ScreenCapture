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
    if (state != ShapeState::ready) {
        imgPatch = mosaicImg.copy(shape.left()*1.5,shape.top()*1.5,shape.width()*1.5,shape.height()*1.5);
    }
    painter->drawImage(shape.topLeft(), imgPatch);
}

bool ShapeMosaicRect::mouseRelease(QMouseEvent* event)
{
    if (pressPos == event->position() && state != ShapeState::moving) { //鼠标按下，没有拖拽，随即释放
        return false;
    }
    if (state >= ShapeState::sizing0) {
        ShapeRectBase::mouseRelease(event);
		return true;
    }
    return false;
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