#include <QPainter>
#include <QWindow>

#include "ShapeMosaicRect.h"
#include "../Win/WinBase.h"
#include "../App/Util.h"

ShapeMosaicRect::ShapeMosaicRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = true;
    auto win = (WinBase*)parent;
    //QImage winImg(win->imgBg);
    //QPainter painter(&winImg);
    //for (int i = 0; i < win->shapes.size(); i++)
    //{
    //    win->shapes[i]->paint(&painter);
    //}
    //mosaicPixs = winImg.scaled(winImg.width() / mosaicRectSize,
    //    winImg.height() / mosaicRectSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

}

ShapeMosaicRect::~ShapeMosaicRect()
{
}

void ShapeMosaicRect::paint(QPainter* painter)
{
    if (state == ShapeState::ready) {
        painter->drawImage(shape.topLeft(), imgPatch);
    }
    else {
        painter->setPen(Qt::NoPen);
        for (quint32 x = shape.left(); x < shape.right(); x += mosaicRectSize)
        {
            for (quint32 y = shape.top(); y < shape.bottom(); y += mosaicRectSize)
            {
                auto c = mosaicPixs.pixelColor(x / mosaicRectSize, y / mosaicRectSize);
                painter->setBrush(c);
                QRect mRect(x, y, mosaicRectSize, mosaicRectSize);
                painter->drawRect(mRect);
            }
        }
    }
}

bool ShapeMosaicRect::mouseRelease(QMouseEvent* event)
{
    imgPatch = QImage(shape.size().toSize(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imgPatch);
    painter.setPen(Qt::NoPen);
    for (quint32 x = shape.left(); x < shape.right(); x += mosaicRectSize)
    {
        for (quint32 y = shape.top(); y < shape.bottom(); y += mosaicRectSize)
        {
            auto c = mosaicPixs.pixelColor(x / mosaicRectSize, y / mosaicRectSize);
            painter.setBrush(c);
            QRect mRect(x- shape.left(), y- shape.top(), mosaicRectSize, mosaicRectSize);
            painter.drawRect(mRect);
        }
    }
    ShapeRectBase::mouseRelease(event); 
    return false;
}
