#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"
#include <QImage>
#include <QDateTime>
#include <QDebug>
#include <cmath>


void MainWindow::initLayer()
{


    scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    auto imgSize = ScreenShoter::Get()->desktopImage.size();
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
    layerBgPainter->drawImage(0, 0, ScreenShoter::Get()->desktopImage);

    layerMosaicImg = new QImage(imgSize, QImage::Format_ARGB32);
    layerMosaicImg->setDevicePixelRatio(scaleFactor);
    layerMosaicImg->fill(0);
    layerMosaicPainter = new QPainter(layerMosaicImg);
    layerMosaicPainter->setRenderHint(QPainter::Antialiasing, true);
    layerMosaicPainter->setPen(Qt::NoPen);
    //layerBgPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);

}

void MainWindow::initMosaic()
{
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
        //todo 还搞个path干啥，直接画方框得了
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
    if (this->textInputBox->isVisible())
    {
        translateTextToPath();
    }
    if (paths.count() < 1) return;    
    if (!isDrawing) return;
    auto& path = paths.last();
    if (path->isEraser)
    {
        layerBgPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        layerBgPainter->drawImage(0, 0, *layerDrawingImg);
        path->initPatch(layerBgImg, scaleFactor);
    }
    else if (path->isMosaic)
    {
        //把马赛克图层拷贝到Drawing图层上
        memcpy(layerDrawingImg->bits(), layerMosaicImg->bits(), layerDrawingImg->sizeInBytes());
        //把bg图层画到drawing图层上，由于bg图层包含镂空的马赛克路径，所以此时drawing图层镂空部分将显示马赛克图案
        layerDrawingPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        layerDrawingPainter->drawImage(0, 0, *layerBgImg);
        //把drawing图层拷贝到bg图层
        memcpy(layerBgImg->bits(), layerDrawingImg->bits(), layerBgImg->sizeInBytes());
        path->initPatch(layerBgImg, scaleFactor);
    }
    else
    {
        paintPath(path, layerBgPainter);
    }
    layerDrawingImg->fill(0);
    showDraggerCount = 0;
    isDrawing = false;    
    repaint();
}



void MainWindow::paintPath(PathModel* path, QPainter* painter)
{
    if (path->needFill)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(path->color));
    }
    else
    {
        painter->setPen(QPen(QBrush(path->color), path->borderWidth, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
    }

    if (path->isEraser || path->isMosaic)
    {
        painter->setCompositionMode(QPainter::CompositionMode_Clear);
        //qDebug() << (path->isMosaic ? "isMosaic" : "isEraser");
    }
    else
    {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    if (path->isEllipse)
    {
        painter->drawEllipse(path->boundingRect());
    }
    else if (path->isText)
    {
        painter->setPen(path->color);
        painter->setFont(path->textFont);
        painter->drawText(path->textRect, Qt::AlignLeft | Qt::AlignTop, path->text, &path->textRect);
    }
    else
    {
        painter->drawPath(*path);
    }

}

PathModel* MainWindow::createPath()
{
    auto path = new PathModel();
    path->color = colorSelector->currentColor();
    paths.append(path);
    return path;
}


void MainWindow::drawArrow(QPointF& curPoint)
{
    qreal height = 26.0, width = 22.0;
    auto& path = paths.last();
    //箭头起点和箭头终点组成的线段，这个线段与水平坐标的夹角
    QLineF tarLine(mousePressPoint, curPoint);
    //角度变弧度，以适应std::cos，std::sin
    qreal v = tarLine.angle() * 3.14159265 / 180;
    // △底边的中点
    qreal centerX = curPoint.x() - height * std::cos(v);
    qreal centerY = curPoint.y() + height * std::sin(v);
    qreal tempA = width / 4 * std::sin(v);
    qreal tempB = width / 4 * std::cos(v);
    // △ 左下的顶点与底边中点之间中间位置的点
    qreal x1 = centerX - tempA;
    qreal y1 = centerY - tempB;
    path->setElementPositionAt(1, x1, y1);
    // △ 左下的顶点
    qreal x2 = x1 - tempA;
    qreal y2 = y1 - tempB;
    path->setElementPositionAt(2, x2, y2);
    // △ 上部顶点，也就是箭头终点
    path->setElementPositionAt(3, curPoint.x(), curPoint.y());
    // △ 右下顶点
    tempA = width / 2 * std::sin(v);
    tempB = width / 2 * std::cos(v);
    qreal x3 = centerX + tempA;
    qreal y3 = centerY + tempB;
    path->setElementPositionAt(4, x3, y3);
    // △ 右下的顶点与底边中点之间中间位置的点
    qreal x4 = centerX + tempA / 2;
    qreal y4 = centerY + tempB / 2;
    path->setElementPositionAt(5, x4, y4);
    layerDrawingImg->fill(0);
    paintPath(path, layerDrawingPainter);
    isDrawing = true;
    repaint();
}
