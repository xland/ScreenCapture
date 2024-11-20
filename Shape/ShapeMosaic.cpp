#include <qpainter.h>
#include <QTransform>
#include <QtConcurrent>
#include <QFuture>
#include <numbers>

#include "ShapeMosaic.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBg.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"

namespace {
    QMap<uint, QColor> mosaicColors;
    QFuture<void> future;
}

ShapeMosaic::ShapeMosaic(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    for (int i = 0; i < 8; i++)
    {
        draggers.push_back(QRect());
    }
    isRect = win->toolSub->getSelectState("eraserFill");
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeMosaic::~ShapeMosaic()
{
}

void ShapeMosaic::initMosaic(WinBg* winBg)
{
    future = QtConcurrent::run([winBg]() {
        int mosaicRectSize = 12;
        int w = winBg->w - mosaicRectSize;
        int h = winBg->h - mosaicRectSize;
        for (int var1 = 0; var1 < w; var1 += mosaicRectSize)
        {
            for (int var2 = 0; var2 < h; var2 += mosaicRectSize)
            {
                //采9个样即可
                auto color0 = winBg->bgImg.pixelColor(var1 + 1, var2 + 1);
                auto color1 = winBg->bgImg.pixelColor(var1 + mosaicRectSize - 1, var2 + 1);
                auto color2 = winBg->bgImg.pixelColor(var1 + mosaicRectSize - 1, var2 + mosaicRectSize - 1);
                auto color3 = winBg->bgImg.pixelColor(var1 + 1, var2 + mosaicRectSize - 1);
                auto color4 = winBg->bgImg.pixelColor(var1 + mosaicRectSize / 2, var2 + mosaicRectSize / 2);
                auto color5 = winBg->bgImg.pixelColor(var1 + 1, var2 + mosaicRectSize / 3);
                auto color6 = winBg->bgImg.pixelColor(var1 + mosaicRectSize / 3, var2 + mosaicRectSize / 3);
                auto color7 = winBg->bgImg.pixelColor(var1 + mosaicRectSize / 3 * 2, var2 + mosaicRectSize / 3 * 2);
                auto color8 = winBg->bgImg.pixelColor(var1 + 1, var2 + mosaicRectSize / 3 * 2);

                auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red() + color5.red() + color6.red() + color7.red() + color8.red();
                auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green() + color5.green() + color6.green() + color7.green() + color8.green();
                auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue() + color4.blue() + color5.blue() + color6.blue() + color7.blue() + color8.blue();
                QColor c(r / 9, g / 9, b / 9);
                mosaicColors[var1 * var2] = c;
            }
        }
        });
}

void ShapeMosaic::initImgPatch()
{
    future.waitForFinished();
    auto rect = path.boundingRect();
    imgPatch = QImage(rect.width(),rect.height(), QImage::Format_ARGB32);
    QPainter painter(&imgPatch);
    rect.left();
    rect.top();
    for (size_t i = 0; i < 11; i++)
    {
        for (size_t i = 0; i < 22; i++)
        {

        }
    }
}

void ShapeMosaic::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeMosaic::paint(QPainter* painter)
{
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    if (path.isEmpty()) {
        painter->drawLine(startPos, endPos);
    }
    else {
        painter->drawPath(path);
    }
    painter->restore();

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
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    if (hoverDraggerIndex == -1) {
        double distance = std::abs(coeffA * pos.x() + coeffB * pos.y() + coeffC) / diffVal;
        if (distance <= strokeWidth / 2) {
            hoverDraggerIndex = 8;
            auto win = (WinBase*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
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
            path.lineTo(event->pos());
            state = ShapeState::sizing0;
        }
    }
    if (path.isEmpty() && hoverDraggerIndex >= 0) {
        pressPos = event->pos().toPointF();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        win->update();
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
    }
    initImgPatch();
    auto win = (WinBase*)parent();
    win->update();
    event->accept();
}
