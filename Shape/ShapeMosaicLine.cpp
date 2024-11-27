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
        painter->drawImage(pathRect, imgPatch);
    }
    else {
        QPainter winPainter(winImg);
        winPainter.setCompositionMode(QPainter::CompositionMode_Clear);
        QPen pen(Qt::transparent);
        pen.setWidth(strokeWidth);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        winPainter.setPen(pen);
        winPainter.setBrush(Qt::NoBrush);
        if (path.isEmpty()) {
            winPainter.drawLine(startPos, endPos);
        }
        else {
            winPainter.drawPath(path);
        }
        painter->drawImage(QPoint(0, 0), *mosaicImg);
        painter->drawImage(QPoint(0, 0), *winImg);
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
    auto win = (WinBase*)(parent());
    auto dpr = mosaicImg->devicePixelRatio();
    auto tarRect = QRectF(pathRect.topLeft() * dpr, pathRect.bottomRight() * dpr).toRect();
    imgPatch = mosaicImg->copy(tarRect);
    QPainter painter(&imgPatch);
    painter.drawImage(QPoint(0, 0), winImg->copy(tarRect));
    delete mosaicImg;
    delete winImg;
}

