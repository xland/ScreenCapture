#include <qpainter.h>
#include <QTransform>
#include <QtConcurrent>
#include <QFuture>
#include <QWindow>
#include <numbers>

#include "ShapeMosaicLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"

ShapeMosaicLine::ShapeMosaicLine(QObject* parent) : ShapeLineBase(parent)
{
    
    auto win = (WinBase*)parent;
    auto imgPair = win->createMosaicImg();
    winImg = imgPair.first;
    mosaicImg = imgPair.second;

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
        auto winImgTemp = winImg->copy();
        clearPath(&winImgTemp);
        painter->drawImage(QPoint(0, 0), *mosaicImg);
        painter->drawImage(QPoint(0, 0), winImgTemp);
    }
}

void ShapeMosaicLine::mouseRelease(QMouseEvent* event)
{
    ShapeLineBase::mouseRelease(event);
    if (path.isEmpty()) {
        pathRect = QRectF(startPos, endPos).normalized().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
    }
    else {
        pathRect = path.boundingRect().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);
    }
    clearPath(winImg);
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
        win->repaint();
        auto imgPair = win->createMosaicImg();
        winImg = imgPair.first;
        mosaicImg = imgPair.second;
    }
    ShapeLineBase::mousePress(event);
}

void ShapeMosaicLine::clearPath(QImage* img)
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

