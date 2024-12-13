#include <qpainter.h>
#include <QTransform>
#include <QtConcurrent>
#include <QFuture>
#include <QWindow>
#include <numbers>

#include "ShapeMosaic.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"

ShapeMosaic::ShapeMosaic(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBox*)parent;
    isRect = win->toolSub->getSelectState("eraserFill");
    if (isRect) {
        prepareDraggers(8);
    }
    strokeWidth = win->toolSub->getStrokeWidth();
    initMosaicBgImg();
}

ShapeMosaic::~ShapeMosaic()
{
}


void ShapeMosaic::initMosaicBgImg()
{
    //auto start = QTime::currentTime();
    auto win = (WinBox*)parent();
    winImg = win->grab().convertToFormat(QImage::Format_ARGB32);
    auto dpr = 1.5; //todo
    winImg.setDevicePixelRatio(dpr);
    {
        QPainter winPainter(&winImg);
        for (int i = 0; i < win->shapes.size(); i++)
        {
            win->shapes[i]->paint(&winPainter);
        }
    } 
    mosaicImg = winImg.copy();
    mosaicImg.setDevicePixelRatio(dpr);
    int mosaicRectSize{ 18 };
    QImage imgTemp = winImg.scaled(winImg.width() / mosaicRectSize,
        winImg.height() / mosaicRectSize, Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);
    auto smallSize = mosaicRectSize / dpr;
    QPainter painter(&mosaicImg);
    for (quint32 x = 0; x < imgTemp.width(); x++)
    {
        for (quint32 y = 0; y < imgTemp.height(); y++)
        {
            auto c = imgTemp.pixelColor(x, y);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            QRectF mRect(x * smallSize, y * smallSize, smallSize, smallSize);
            painter.drawRect(mRect);
        }
    }
    //int elapsedMilliseconds = start.msecsTo(QTime::currentTime());
}

void ShapeMosaic::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeMosaic::paint(QPainter* painter)
{
    if (state == ShapeState::ready) {
        painter->drawImage(pathRect, imgPatch);
    }
    else {
        painter->drawImage(QPoint(0, 0), mosaicImg);
        QPainter winPainter(&winImg);
        winPainter.setCompositionMode(QPainter::CompositionMode_Clear);
        QPen pen(color);
        pen.setWidth(strokeWidth);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        winPainter.setPen(pen);
        winPainter.setBrush(Qt::NoBrush);
        winPainter.drawPath(path);
        painter->drawImage(QPoint(0, 0), winImg);
    }
}
void ShapeMosaic::paintDragger(QPainter* painter)
{
    if (path.isEmpty()) {
        painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(draggers[0]);
        painter->drawRect(draggers[1]);
    }
}
void ShapeMosaic::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    if (!path.isEmpty()) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    if (hoverDraggerIndex == -1) {
        double distance = std::abs(coeffA * pos.x() + coeffB * pos.y() + coeffC) / diffVal;
        if (distance <= strokeWidth / 2) {
            hoverDraggerIndex = 8;
            auto win = (WinBox*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBox*)parent();
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeMosaic::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        auto flag = event->modifiers() & Qt::ShiftModifier;
        if (flag) {
            startPos = event->pos().toPointF();
            endPos = startPos;
            hoverDraggerIndex = 1;
        }
        else {
            path.moveTo(event->pos());
            state = ShapeState::sizing0;
        }
    }
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        auto win = (WinBox*)parent();
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeMosaic::mouseRelease(QMouseEvent* event)
{
    if (path.isEmpty()) {
        if (state >= ShapeState::sizing0) {
            resetDragger();
            coeffA = startPos.y() - endPos.y();
            coeffB = endPos.x() - startPos.x();
            coeffC = startPos.x() * endPos.y() - endPos.x() * startPos.y();
            diffVal = std::sqrt(coeffA * coeffA + coeffB * coeffB);
            state = ShapeState::ready;
            event->accept();
        }
    }
    else {
        state = ShapeState::ready;
        pathRect = path.boundingRect().toRect().adjusted(-strokeWidth,-strokeWidth,strokeWidth,strokeWidth);
        auto win = (WinBox*)(parent());
        auto dpr = 1.5; //todo
        auto tarRect = QRect(pathRect.topLeft()*dpr, pathRect.bottomRight()*dpr);
        imgPatch = mosaicImg.copy(tarRect);
        QPainter painter(&imgPatch);
        painter.drawImage(QPoint(0, 0), winImg.copy(tarRect));
        {
            QImage emptyImage1;
            mosaicImg.swap(emptyImage1);
            QImage emptyImage2;
            winImg.swap(emptyImage2);
        }
        win->refreshBoard();
        event->accept();
    }
}
void ShapeMosaic::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (path.elementCount() <= 0) {  //这里不能判断isEmpty
        if (state == ShapeState::sizing0) {
            startPos = event->pos();
        }
        if (state == ShapeState::sizing1) {
            endPos = event->pos();
        }
        else if (state == ShapeState::moving) {
            auto pos = event->pos();
            auto span = pos - pressPos;
            startPos += span;
            endPos += span;
            pressPos = pos;
        }
    }
    else {
        path.lineTo(event->position());
        auto win = (WinBox*)parent();
        win->refreshCanvas(this, true);
        event->accept();
    }
}
