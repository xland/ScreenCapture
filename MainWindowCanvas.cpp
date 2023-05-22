#include "MainWindow.h"
#include "ScreenShoter.h"
#include <QImage>
#include <QDateTime>
#include <QDebug>


void MainWindow::initCanvasImg()
{
    auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
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
    layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
//  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
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
    paintPath(maskPath, &p);
}

void MainWindow::endOneDraw()
{

    isDrawing = false;
    showDraggerCount = 0;
    if (paths.count() > 0)
    {
        auto& path = paths.last();
        if (!path.isEraser && !path.isMosaic)
        {
            paintPath(path, layerBgPainter);
        }
    }
}

void MainWindow::initMosaicImg()
{
    //todo 优化空间，线性扫描，逐行绘制
//    auto start = QDateTime::currentDateTime();
//    auto w = layerDrawingImg->width();
//    auto h = layerDrawingImg->height();
//    int mosaicRectSize = 8;
//    layerBgPainter->setPen(Qt::NoPen);
//    layerBgPainter->drawImage(0, 0, *layerDrawingImg);
//    for (int var1 = 0; var1 < w; var1 += mosaicRectSize)
//    {
//        for (int var2 = 0; var2 < h; var2 += mosaicRectSize)
//        {
//            auto color0 = layerDrawingImg->pixelColor(var1 + 4, var2 + 4);
//            auto color1 = layerDrawingImg->pixelColor(var1 + 1, var2 + 1);
//            auto color2 = layerDrawingImg->pixelColor(var1 + 7, var2 + 1);
//            auto color3 = layerDrawingImg->pixelColor(var1 + 7, var2 + 7);
//            auto color4 = layerDrawingImg->pixelColor(var1 + 1, var2 + 7);
//            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red();
//            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green();
//            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue();
//            QColor c(r / 5, g / 5, b / 5);
//            layerBgPainter->setBrush(QBrush(c));
//            layerBgPainter->drawRect(QRectF(var1, var2, mosaicRectSize, mosaicRectSize));
//            qDebug() << var1 << var2;
//        }
//    }
//    qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch() - start.toMSecsSinceEpoch();
    endOneDraw();
    memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
    int mosaicRectSize = 6;
    auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    auto imgSize = ScreenShoter::Get()->screenRects[0].size() * scaleFactor;
    QImage mosaicImg(imgSize, QImage::Format_ARGB32);
    QPainter painter(&mosaicImg);
    painter.drawImage(0, 0, *layerDrawingImg);
    painter.setPen(Qt::NoPen);
    for (int var1 = 0; var1 < imgSize.width(); var1 += mosaicRectSize)
    {
        for (int var2 = 0; var2 < imgSize.height(); var2 += mosaicRectSize)
        {
            QRect rect(var1, var2, mosaicRectSize, mosaicRectSize);
            auto color0 = mosaicImg.pixelColor(rect.center());
            auto color1 = mosaicImg.pixelColor(rect.topLeft());
            auto color2 = mosaicImg.pixelColor(rect.topRight());
            auto color3 = mosaicImg.pixelColor(rect.bottomRight());
            auto color4 = mosaicImg.pixelColor(rect.bottomLeft());
            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red();
            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green();
            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue();
            QColor c(r / 5, g / 5, b / 5);
            painter.setBrush(QBrush(c));
            painter.drawRect(rect);
        }
    }
    layerBgPainter->drawImage(0, 0, mosaicImg);
//    qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch() - start.toMSecsSinceEpoch();
}

void MainWindow::paintPath(PathModel& path, QPainter* painter)
{
    if (path.needBorder)
    {
        painter->setPen(QPen(QBrush(path.borderColor), path.borderWidth));
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }
    if (path.needFill)
    {
        painter->setBrush(QBrush(path.fillColor));
    }
    else
    {
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
