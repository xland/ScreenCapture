#include "MainWindow.h"
#include "ScreenShoter.h"


void MainWindow::initCanvasImg()
{
    auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    canvasImg1 = new QImage(ScreenShoter::Get()->screenRects[0].size()*scaleFactor, QImage::Format_ARGB32_Premultiplied);
    canvasImg1->setDevicePixelRatio(scaleFactor);
    canvasImg1->fill(0);
    canvasImg2 = new QImage(ScreenShoter::Get()->screenRects[0].size()*scaleFactor, QImage::Format_ARGB32_Premultiplied);
    canvasImg2->setDevicePixelRatio(scaleFactor);
    canvasImg2->fill(0);
    painter1 = new QPainter(canvasImg1);
    painter1->setRenderHint(QPainter::Antialiasing, true);
    //    painter1->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter2 = new QPainter(canvasImg2);
    painter2->setRenderHint(QPainter::Antialiasing, true);
    //    painter2->setRenderHint(QPainter::SmoothPixmapTransform, false);
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    if (showDraggerCount > 0)
    {
        QPainterPath draggerPath;
        for (int var = 0; var < showDraggerCount; ++var)
        {
            draggerPath.addRect(dragers[var]);
        }
        painter1->setPen(QPen(QBrush(Qt::black), 0.5));
        painter1->setBrush(Qt::NoBrush);
        painter1->drawPath(draggerPath);
    }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    p.drawImage(0, 0, *canvasImg1);
    paintPath(maskPath, &p);
    p.end();
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
