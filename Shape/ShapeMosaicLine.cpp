#include <QPainter>
#include <QWindow>

#include "ShapeMosaicLine.h"
#include "../Win/WinBox.h"

ShapeMosaicLine::ShapeMosaicLine(QObject* parent) : ShapeLineBase(parent)
{
    
    auto win = (WinBox*)parent;
    createMosaicImg();

}
ShapeMosaicLine::~ShapeMosaicLine()
{
}

void ShapeMosaicLine::paint(QPainter* painter)
{
    if (state == ShapeState::ready) {
        painter->drawImage(pathRect.topLeft(), imgPatch);
    }
    else {
        if (path.isEmpty()) {
            auto winImgTemp = winImg->copy();
            erasePath(&winImgTemp);
            painter->drawImage(QPoint(0, 0), *mosaicImg);
            painter->drawImage(QPoint(0, 0), winImgTemp);
        }
        else {
            erasePath(winImg);
            painter->drawImage(QPoint(0, 0), *mosaicImg);
            painter->drawImage(QPoint(0, 0), *winImg);
        }
    }
}

void ShapeMosaicLine::mouseRelease(QMouseEvent* event)
{
    ShapeLineBase::mouseRelease(event);
    if (path.isEmpty()) {
        pathRect = QRectF(startPos, endPos).normalized().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
        erasePath(winImg);
    }
    else {
        pathRect = path.boundingRect().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
    }
    auto win = (WinBase*)(parent());
    auto dpr = mosaicImg->devicePixelRatio();
    auto tarRect = QRectF(pathRect.topLeft() * dpr, pathRect.bottomRight() * dpr).toRect();
    imgPatch = mosaicImg->copy(tarRect);
    QPainter painter(&imgPatch);
    painter.drawImage(QPoint(0, 0), winImg->copy(tarRect));
    delete mosaicImg;
    delete winImg;
}

void ShapeMosaicLine::mousePress(QMouseEvent* event)
{
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        imgPatch = QImage(0, 0);
        auto win = (WinBase*)parent();
        //win->repaint();
        createMosaicImg();
    }
    ShapeLineBase::mousePress(event);
}

void ShapeMosaicLine::erasePath(QImage* img)
{
    QPainter painter(img);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    if (path.isEmpty()) {
        painter.drawLine(startPos, endPos);
    }
    else {
        painter.drawPath(path);
    }
}

void ShapeMosaicLine::createMosaicImg()
{
    auto win = (WinBox*)parent();
    //auto start = QTime::currentTime();
    winImg = new QImage(win->img);
    {
        QPainter painter(winImg);
        for (int i = 0; i < win->shapes.size(); i++)
        {
            win->shapes[i]->paint(&painter);
        }
    }
    QImage mosaicPixs = winImg->scaled(winImg->width() / mosaicRectSize,
        winImg->height() / mosaicRectSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    auto smallSize = mosaicRectSize / winImg->devicePixelRatio();
    mosaicImg = new QImage(win->img);
    QPainter painter(mosaicImg);
    painter.setPen(Qt::NoPen);
    //qDebug() << "\n create" << start.msecsTo(QTime::currentTime());
    for (quint32 x = 0; x < mosaicPixs.width(); x++)
    {
        auto xPos = x * smallSize;
        for (quint32 y = 0; y < mosaicPixs.height(); y++)
        {
            auto c = mosaicPixs.pixelColor(x, y);
            painter.setBrush(c);
            QRectF mRect(xPos, y * smallSize, smallSize, smallSize);
            painter.drawRect(mRect);
        }
    }
    //qDebug() << "\n create" << start.msecsTo(QTime::currentTime());
    //return { winImg,mosaicImg };
}

