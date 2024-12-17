#include <QPainter>
#include <QWindow>

#include "ShapeMosaicRect.h"
#include "../Win/WinBox.h"

ShapeMosaicRect::ShapeMosaicRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = true;
    auto win = ((WinBox*)parent);
    QImage winImg(win->img);
    QPainter painter(&winImg);
    for (int i = 0; i < win->shapes.size(); i++)
    {
        win->shapes[i]->paint(&painter);
    }
    mosaicPixs = winImg.scaled(winImg.width() / mosaicRectSize,
        winImg.height() / mosaicRectSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

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
        auto dpr = 1.5; //todo
        auto smallSize = mosaicRectSize / dpr;
        painter->setPen(Qt::NoPen);
        for (quint32 x = shape.left(); x < shape.right(); x += smallSize)
        {
            for (quint32 y = shape.top(); y < shape.bottom(); y += smallSize)
            {
                auto c = mosaicPixs.pixelColor(x / smallSize, y / smallSize);
                painter->setBrush(c);
                QRect mRect(x, y, smallSize + 1, smallSize + 1);
                painter->drawRect(mRect);
            }
        }
    }
}

void ShapeMosaicRect::mouseRelease(QMouseEvent* event)
{
    ShapeRectBase::mouseRelease(event);
    imgPatch = QImage(shape.size().toSize(), QImage::Format_ARGB32);
    QPainter painter(&imgPatch);
    auto dpr = 1.5; //todo
    auto smallSize = mosaicRectSize / dpr;
    painter.setPen(Qt::NoPen);
    for (quint32 x = shape.left(); x < shape.right(); x += smallSize)
    {
        for (quint32 y = shape.top(); y < shape.bottom(); y += smallSize)
        {
            auto c = mosaicPixs.pixelColor(x / smallSize, y / smallSize);
            painter.setBrush(c);
            QRect mRect(x- shape.left(), y- shape.top(), smallSize + 1, smallSize + 1);
            painter.drawRect(mRect);
        }
    }
}

void ShapeMosaicRect::paintMosicRects(QPainter* painter)
{

}
