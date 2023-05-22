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
    //    layerDrawingPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);

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
    if (paths.count() > 0)
    {
        auto& path = paths.last();
        if (!path.isEraser)
        {
            paintPath(path, layerBgPainter);
            isDrawing = false;
            showDraggerCount = 0;
        }
    }
}

void MainWindow::initMosaicImg()
{
    auto start = QDateTime::currentDateTime();
    int mosaicRectSize = 6;
    auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    auto imgSize = ScreenShoter::Get()->screenRects[0].size() * scaleFactor;
    mosaicImg = new QImage(imgSize, QImage::Format_ARGB32);
    QPainter painter(mosaicImg);
    painter.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    painter.drawImage(0, 0, *layerDrawingImg);
    painter.setPen(Qt::NoPen);
    for (int var1 = 0; var1 < imgSize.width(); var1 += mosaicRectSize)
    {
        for (int var2 = 0; var2 < imgSize.height(); var2 += mosaicRectSize)
        {
            QRect rect(var1, var2, mosaicRectSize, mosaicRectSize);
            auto color0 = mosaicImg->pixelColor(rect.center());
            auto color1 = mosaicImg->pixelColor(rect.topLeft());
            auto color2 = mosaicImg->pixelColor(rect.topRight());
            auto color3 = mosaicImg->pixelColor(rect.bottomRight());
            auto color4 = mosaicImg->pixelColor(rect.bottomLeft());
            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red();
            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green();
            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue();
            QColor c(r / 5, g / 5, b / 5);
            painter.setBrush(QBrush(c));
            painter.drawRect(rect);
        }
    }
    layerDrawingPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    layerDrawingPainter->drawImage(0, 0, *layerDrawingImg);
    qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch() - start.toMSecsSinceEpoch();
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
    if (path.isEraser)
    {
        painter->setCompositionMode(QPainter::CompositionMode_Clear);
    }
    else
    {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    painter->drawPath(path);
}

void MainWindow::paintMosaic(PathModel& path, QPainter* painter, qreal x1, qreal y1, qreal x2, qreal y2)
{
//    QPainter bgPainter(path.mosaicBg);
//    painter->setPen(QPen(QBrush(Qt::black), path.borderWidth));
//    painter->setBrush(Qt::NoBrush);
    painter->setClipPath(path);
    painter->drawImage(0, 0, *path.mosaicBg);
//    painter->save();
//    painter->setPen(Qt::NoPen);

//    auto angle = (QLineF(QPointF(x1, y1), QPointF(x2, y2))).angle();

//    QTransform transform;
//    transform.rotate(0 - angle);

//    for (int var1 = x1; var1 < x2; var1 += path.mosaicRectSize)
//    {
//        for (int var2 = y1 - path.mosaicWidth / 2; var2 < y2 + path.mosaicWidth / 2; var2 += path.mosaicRectSize)
//        {
//            QRect rect(var1, var2, path.mosaicRectSize, path.mosaicRectSize);
//            auto polygon = transform.mapToPolygon(rect);
////            auto p = transform.map(rect.center());
////            auto color = path.mosaicBg->pixelColor(p);
////            qDebug() << color.red() << color.green() << color.blue() << color.alpha();
//            painter->setBrush(QBrush(Qt::black));
//            painter->drawPolygon(polygon);
//        }
//    }
}
