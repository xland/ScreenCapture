#include "PathModel.h"
#include "ScreenShoter.h"
#include <QPainter>
#include <QDateTime>
#include <QDebug>

PathModel::PathModel(): isEraser{false}
{

}

void PathModel::resetPoint5()
{
    qreal x2 = -999.0, x1 = 999999999.0;
    qreal y2 = -999.0, y1 = 999999999.0;
    for (int var = 0; var < 5; ++var)
    {
        auto ele = elementAt(var);
        if (ele.x < x1)
        {
            x1 = ele.x;
        }
        if (ele.x > x2)
        {
            x2 = ele.x;
        }
        if (ele.y < y1)
        {
            y1 = ele.y;
        }
        if (ele.y > y2)
        {
            y2 = ele.y;
        }
    }
    setElementPositionAt(0, x1, y1);
    setElementPositionAt(1, x2, y1);
    setElementPositionAt(2, x2, y2);
    setElementPositionAt(3, x1, y2);
    setElementPositionAt(4, x1, y1);
}

void PathModel::initMosaicBg(qreal scaleFactor, QImage* curImg)
{
    auto start = QDateTime::currentDateTime();
    auto imgSize = ScreenShoter::Get()->screenRects[0].size() * scaleFactor;
    mosaicBg = new QImage(imgSize, QImage::Format_ARGB32);
    QPainter painter(mosaicBg);
    painter.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    painter.drawImage(0, 0, *curImg);
    painter.setPen(Qt::NoPen);
    for (int var1 = 0; var1 < imgSize.width(); var1 += mosaicRectSize)
    {
        for (int var2 = 0; var2 < imgSize.height(); var2 += mosaicRectSize)
        {
            QRect rect(var1, var2, mosaicRectSize, mosaicRectSize);
            auto color0 = mosaicBg->pixelColor(rect.center());
            auto color1 = mosaicBg->pixelColor(rect.topLeft());
            auto color2 = mosaicBg->pixelColor(rect.topRight());
            auto color3 = mosaicBg->pixelColor(rect.bottomRight());
            auto color4 = mosaicBg->pixelColor(rect.bottomLeft());
            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red();
            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green();
            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue();
            QColor c(r / 5, g / 5, b / 5);
            painter.setBrush(QBrush(c));
            painter.drawRect(rect);
        }
    }
    qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch() - start.toMSecsSinceEpoch();

}
