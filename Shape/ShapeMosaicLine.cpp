#include <QPainter>
#include <QWindow>
#include <QTime>

#include "ShapeMosaicLine.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"
#include "../App/Util.h"
#include "Shape/ShapeBase.h"

ShapeMosaicLine::ShapeMosaicLine(QObject* parent) : ShapeLineBase(parent)
{
    createMosaicImg();
}
ShapeMosaicLine::~ShapeMosaicLine()
{
}

void ShapeMosaicLine::paint(QPainter* painter)
{
    if (state == ShapeState::ready) {
        auto win = (WinBase*)parent();
        painter->drawImage(pathRect.topLeft()/win->devicePixelRatio(), imgPatch);
    }
    else if(state == ShapeState::moving || isStraight){
        auto tempImg = winImg.copy();
        erasePath(&tempImg);
        painter->drawImage(QPoint(0, 0), mosaicImg);
        painter->drawImage(QPoint(0, 0), tempImg);
    }
    else {
        erasePath(&winImg);
        painter->drawImage(QPoint(0, 0), mosaicImg);//先画马赛克图
        painter->drawImage(QPoint(0, 0), winImg);   //再画擦除后的背景图,这样擦除部分就会显示马赛克图像了
    }
}

bool ShapeMosaicLine::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        pressPos = event->position();
        isStraight = event->modifiers() & Qt::ShiftModifier;
        state = (ShapeState)((int)ShapeState::sizing0 + 1);
        path.moveTo(pressPos);
        return true;
    }
    else if (hoverDraggerIndex >= 0) {
        imgPatch = QImage();
        createMosaicImg();
        pressPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        if (hoverDraggerIndex == 0) {
            pathStart.moveTo(pressPos);
        }
        return true;
    }
    return false;
}
bool ShapeMosaicLine::mouseRelease(QMouseEvent* event)
{
    if (path.isEmpty()) {
        return false;
    }
	auto win = (WinBase*)parent();
    pathRect = path.boundingRect().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth);

    auto dpr = win->devicePixelRatio();
    auto tl = pathRect.topLeft() * dpr;
    auto br = pathRect.bottomRight() * dpr;
    pathRect.setCoords(tl.x(), tl.y(), br.x(), br.y());

    imgPatch = mosaicImg.copy(pathRect.toRect());
    QPainter painter(&imgPatch);
    if (state == ShapeState::moving || isStraight) {
        auto tempImg = winImg.copy();
        erasePath(&tempImg);
        painter.drawImage(QPoint(0, 0), tempImg.copy(pathRect.toRect()));
    }
    else {
        painter.drawImage(QPoint(0, 0), winImg.copy(pathRect.toRect()));
    }
    mosaicImg = QImage();
    winImg = QImage();
    if (state == ShapeState::sizing0) {
        QPainterPath tempPath;
        auto ele = pathStart.elementAt(pathStart.elementCount() - 1);
        tempPath.moveTo(ele.x, ele.y);
        for (int i = pathStart.elementCount() - 2; i >= 0; --i)
        {
            auto ele = pathStart.elementAt(i);
            tempPath.lineTo(ele.x, ele.y);
        }
        pathStart.clear();
        for (int i = 0; i < path.elementCount(); i++)
        {
            auto ele = path.elementAt(i);
            tempPath.lineTo(ele.x, ele.y);
        }
        path = tempPath;
    }
    auto startPos = path.elementAt(0);
    auto endPos = path.elementAt(path.elementCount() - 1);
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x - half, startPos.y - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x - half, endPos.y - half, draggerSize, draggerSize);
    pathStroker = stroker.createStroke(path);
    state = ShapeState::ready;
    return true;
}


void ShapeMosaicLine::erasePath(QImage* img)
{
    QPainter p(img);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.drawPath(path);
}

void ShapeMosaicLine::createMosaicImg()
{
    auto win = (WinBase*)parent();
    winImg = win->canvas->imgBg.copy();
    {
        QPainter painter(&winImg);
        for (auto& s:win->canvas->shapes)
        {
			s->paint(&painter);
        }
    }
	winImg.setDevicePixelRatio(1.0);
    QImage mosaicPixs = winImg.scaled(winImg.width() / mosaicRectSize,winImg.height() / mosaicRectSize, 
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    mosaicImg = winImg.copy();
    QPainter painter(&mosaicImg);
    painter.setPen(Qt::NoPen);
    for (int x = 0; x < mosaicPixs.width(); x++)
    {
        auto xPos = x * mosaicRectSize;
        for (int y = 0; y < mosaicPixs.height(); y++)
        {
            auto c = mosaicPixs.pixelColor(x, y);
            painter.setBrush(c);
            QRectF mRect(xPos, y * mosaicRectSize, mosaicRectSize, mosaicRectSize);
            painter.drawRect(mRect);
        }
    }
    auto dpr = win->canvas->imgBg.devicePixelRatio();
    mosaicImg.setDevicePixelRatio(dpr);
    winImg.setDevicePixelRatio(dpr);
}

