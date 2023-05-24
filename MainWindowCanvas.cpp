#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"
#include <QImage>
#include <QDateTime>
#include <QDebug>


void MainWindow::initLayer()
{
    scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    auto imgSize = ScreenShoter::Get()->screenRects[0].size() * scaleFactor;
    layerDrawingImg = new QImage(imgSize, QImage::Format_ARGB32);
    layerDrawingImg->setDevicePixelRatio(scaleFactor);
    layerDrawingImg->fill(0);
    layerDrawingPainter = new QPainter(layerDrawingImg);
    layerDrawingPainter->setRenderHint(QPainter::Antialiasing, true);
    //layerDrawingPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    layerBgImg = new QImage(imgSize, QImage::Format_ARGB32);
    layerBgImg->setDevicePixelRatio(scaleFactor);
    layerBgPainter = new QPainter(layerBgImg);
    layerBgPainter->setRenderHint(QPainter::Antialiasing, true);
    //layerBgPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    layerBgPainter->drawImage(0, 0, ScreenShoter::Get()->desktopImages[0]);

    layerMosaicImg = new QImage(imgSize, QImage::Format_ARGB32);
    layerMosaicImg->setDevicePixelRatio(scaleFactor);
    layerMosaicImg->fill(0);
    layerMosaicPainter = new QPainter(layerMosaicImg);
    layerMosaicPainter->setRenderHint(QPainter::Antialiasing, true);
    layerMosaicPainter->setPen(Qt::NoPen);
    //layerBgPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);

}

void MainWindow::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
//  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if (state == "Mosaic")
    {
        p.drawImage(0, 0, *layerMosaicImg);
    }
    p.drawImage(0, 0, *layerBgImg);
    if (isDrawing)
    {
        p.drawImage(0, 0, *layerDrawingImg);
    }
    if (showDraggerCount > 0)
    {
        QPainterPath draggerPath;
        for (int var = 0; var < showDraggerCount; ++var)
        {
            draggerPath.addRect(dragers[var]);
        }
        p.setPen(QPen(QBrush(Qt::black), 0.6));
        p.setBrush(Qt::NoBrush);
        p.drawPath(draggerPath);
    }
    p.setPen(QPen(QBrush(QColor(22, 119, 255)), maskBorderWidth));
    p.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.drawPath(maskPath);
}

void MainWindow::endOneDraw()
{
    isDrawing = false;
    showDraggerCount = 0;
    if (paths.count() > 0)
    {
        auto& path = paths.last();
        if (path.isEraser)
        {
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
        }
        else if (path.isMosaic)
        {
            memcpy(layerDrawingImg->bits(), layerMosaicImg->bits(), layerDrawingImg->sizeInBytes());
            layerDrawingPainter->drawImage(0, 0, *layerBgImg);
            memcpy(layerBgImg->bits(), layerDrawingImg->bits(), layerBgImg->sizeInBytes());
            layerDrawingImg->fill(0);
        }
        else
        {
            paintPath(path, layerBgPainter);
        }
        auto rect = path.controlPointRect().toRect();
        auto point  = rect.topLeft();
        point.setX(point.x() - path.borderWidth);
        point.setY(point.y() - path.borderWidth);
        point = point * scaleFactor;
        rect.moveTo(point);
        rect.setWidth(rect.width() + path.borderWidth * 2);
        rect.setHeight(rect.height() + path.borderWidth * 2);
        rect.setSize(rect.size() * scaleFactor);
        auto img =  layerBgImg->copy(rect);
        historyImgs.append(img);
        historyPoints.append(point);
        img.save("part.png");
//        historyImgs.append(layerBgImg->copy());
//        path.controlPointRect()
    }
}

void MainWindow::initMosaic()
{
    endOneDraw();
    int mosaicRectSize = 6;
    for (int var1 = 0; var1 < layerBgImg->width(); var1 += mosaicRectSize)
    {
        for (int var2 = 0; var2 < layerBgImg->height(); var2 += mosaicRectSize)
        {
            auto color0 = layerBgImg->pixelColor(var1 + 1, var2 + 1);
            auto color1 = layerBgImg->pixelColor(var1 + mosaicRectSize - 1, var2 + 1);
            auto color2 = layerBgImg->pixelColor(var1 + mosaicRectSize - 1, var2 + mosaicRectSize - 1);
            auto color3 = layerBgImg->pixelColor(var1 + 1, var2 + mosaicRectSize - 1);
            auto color4 = layerBgImg->pixelColor(var1 + mosaicRectSize / 2, var2 + mosaicRectSize / 2);
            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red();
            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green();
            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue();
            QColor c(r / 5, g / 5, b / 5);
            for (int var3 = 0; var3 < mosaicRectSize; ++var3)
            {
                for (int var4 = 0; var4 < mosaicRectSize; ++var4)
                {
                    layerMosaicImg->setPixelColor(var1 + var3, var2 + var4, c);
                }
            }
        }
    }
}

void MainWindow::paintPath(PathModel& path, QPainter* painter)
{
    if (path.needFill)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(path.color));
    }
    else
    {
        painter->setPen(QPen(QBrush(path.color), path.borderWidth, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
    }
    if (path.isEraser || path.isMosaic)
    {
        painter->setCompositionMode(QPainter::CompositionMode_Clear);
    }
    else
    {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    painter->drawPath(path);
}

PathModel& MainWindow::createPath()
{
    PathModel path;
    path.color = colorSelector->currentColor();
    paths.append(path);
    return paths.last();
}
