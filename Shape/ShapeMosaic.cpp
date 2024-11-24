#include <qpainter.h>
#include <QTransform>
#include <QtConcurrent>
#include <QFuture>
#include <QWindow>
#include <numbers>

#include "ShapeMosaic.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBg.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"

namespace {
    int mosaicRectSize{ 18 };
}

ShapeMosaic::ShapeMosaic(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    isRect = win->toolSub->getSelectState("eraserFill");
    if (isRect) {
        for (int i = 0; i < 8; i++)
        {
            draggers.push_back(QRect());
        }
    }
    strokeWidth = win->toolSub->getStrokeWidth();



    auto start = QTime::currentTime();
    auto base = (WinBase*)parent;


    auto dpr = base->windowHandle()->devicePixelRatio();
    imgScreen = base->winBg->bgImg.copy();
    imgScreen.setDevicePixelRatio(dpr);
    QPainter painter(&imgScreen);
    for (int i = 0; i < base->shapes.size(); i++)
    {
        base->shapes[i]->paint(&painter);
    }
    QImage imgTemp = imgScreen.scaled(imgScreen.width() / mosaicRectSize, 
        imgScreen.height() / mosaicRectSize, Qt::IgnoreAspectRatio, 
        Qt::SmoothTransformation);    
    auto smallSize = mosaicRectSize / dpr;    
    for (uint x = 0; x < imgTemp.width(); x++)
    {
        for (uint y = 0; y < imgTemp.height(); y++)
        {
            auto c = imgTemp.pixelColor(x, y);
            painter.setBrush(c);
            painter.setPen(Qt::NoPen);
            QRect mRect(x* smallSize, y* smallSize, smallSize, smallSize);
            painter.drawRect(mRect);
        }
    }
    int elapsedMilliseconds = start.msecsTo(QTime::currentTime());
    imgScreen.save("allen123.png");
}

ShapeMosaic::~ShapeMosaic()
{
}

void ShapeMosaic::initImgPatch()
{
    //future.waitForFinished();
    auto rect = path.boundingRect().toRect();
    if (rect.width() <= mosaicRectSize || rect.height() <= mosaicRectSize) {
        return;
    }
    auto base = (WinBase*)parent();
    auto sf = base->windowHandle()->devicePixelRatio();
    auto winBg = base->winBg;
    auto l{ rect.left() * sf }; auto t{ rect.top() * sf }; auto w{ rect.width() * sf }; auto h{ rect.height() * sf };
    imgPatch = QImage(w, h, QImage::Format_ARGB32);
    QPainter painter(&imgPatch);
    painter.drawImage(imgPatch.rect(), winBg->bgImg.copy(l, t, w, h));
    painter.save();
    painter.translate(-l,-t);
    painter.scale(sf, sf);
    base->render(&painter);
    painter.restore();
    for (uint x=0;x<imgPatch.width();x+=mosaicRectSize)
    {   
        for (uint y=0; y < imgPatch.height(); x += mosaicRectSize)
        {
            //一个格子采9个样即可
            auto color0 = getColor(x + 1, y + 1, imgPatch);
            auto color1 = getColor(x + mosaicRectSize - 1, y + 1, imgPatch);
            auto color2 = getColor(x + mosaicRectSize - 1, y + mosaicRectSize - 1, imgPatch);
            auto color3 = getColor(x + 1, y + mosaicRectSize - 1, imgPatch);
            auto color4 = getColor(x + mosaicRectSize / 2, y + mosaicRectSize / 2, imgPatch);
            auto color5 = getColor(x + 1, y + mosaicRectSize / 3, imgPatch);
            auto color6 = getColor(x + mosaicRectSize / 3, y + mosaicRectSize / 3, imgPatch);
            auto color7 = getColor(x + mosaicRectSize / 3 * 2, y + mosaicRectSize / 3 * 2, imgPatch);
            auto color8 = getColor(x + 1, y + mosaicRectSize / 3 * 2, imgPatch);

            auto r = color0.red() + color1.red() + color2.red() + color3.red() + color4.red() + color5.red() + color6.red() + color7.red() + color8.red();
            auto g = color0.green() + color1.green() + color2.green() + color3.green() + color4.green() + color5.green() + color6.green() + color7.green() + color8.green();
            auto b = color0.blue() + color1.blue() + color2.blue() + color3.blue() + color4.blue() + color5.blue() + color6.blue() + color7.blue() + color8.blue();
            auto a = color0.alpha() + color1.alpha() + color2.alpha() + color3.alpha() + color4.alpha() + color5.alpha() + color6.alpha() + color7.alpha() + color8.alpha();
            painter.setBrush(QColor(r / 9, g / 9, b / 9, a / 9));
            painter.setPen(Qt::NoPen);
            QRect rrr(x, y, mosaicRectSize, mosaicRectSize);
            painter.drawRect(rrr);
            if (y > 100 && x > 100) {
                imgPatch.save("allen123.png");
            }            
        }
    }
}

void ShapeMosaic::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

//void ShapeMosaic::drawRectsByPoints(const QVector<QPointF>& points)
//{
//    int rowNum = std::ceil((float)win->w / size);
//    int rectNum = std::ceil(strokeWidth * 2 / size);
//    std::map<int, SkColor> colorCache;
//    SkPaint paint;
//    for (auto& point : points)
//    {
//        int xIndex = (point.fX - strokeWidth) / size;
//        int yIndex = (point.fY - strokeWidth) / size;
//        SkColor4f colorSum = { 0, 0, 0, 0 };
//        for (size_t i = yIndex; i < yIndex + rectNum; i++)
//        {
//            for (size_t j = xIndex; j < xIndex + rectNum; j++)
//            {
//                int key = i * rowNum + j;
//                auto x = j * size;
//                auto y = i * size;
//                if (colorCache.contains(key)) {
//                    paint.setColor(colorCache[key]);
//                    canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
//                }
//                else {
//                    int count{ 0 };
//                    for (size_t x1 = x; x1 <= x + size; x1 += 2)
//                    {
//                        for (size_t y1 = y; y1 <= y + size; y1 += 2)
//                        {
//                            SkColor4f currentColor;
//                            if (x1 >= canvas->getSurface()->width() || y1 >= canvas->getSurface()->height()) {
//                                currentColor = SkColor4f::FromColor(SK_ColorBLACK);
//                            }
//                            else {
//                                auto colorSrc = win->pixSrc->getColor(x1, y1);
//                                auto colorBack = win->pixBack->getColor(x1, y1);
//                                if (colorBack == SK_ColorTRANSPARENT) {
//                                    currentColor = SkColor4f::FromColor(colorSrc);
//                                }
//                                else {
//                                    currentColor = ColorBlender::Blend(colorSrc, colorBack);
//                                }
//                            }
//                            colorSum.fR += currentColor.fR;
//                            colorSum.fG += currentColor.fG;
//                            colorSum.fB += currentColor.fB;
//                            count++;
//                        }
//                    }
//                    colorSum.fR /= count;
//                    colorSum.fG /= count;
//                    colorSum.fB /= count;
//                    colorSum.fA = 255;
//                    auto color = colorSum.toSkColor();
//                    paint.setColor(color);
//                    canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
//                    colorCache.insert({ key, color });
//                }
//            }
//        }
//    }
//}

QColor ShapeMosaic::getColor(int x, int y, const QImage& img)
{
    if (x > img.width()) {
        x = img.width() - 1;
    }
    else if (y > img.height()) {
        y = img.height() - 1;
    }
    return img.pixelColor(x, y);
}

void ShapeMosaic::paint(QPainter* painter)
{
    auto rect = path.boundingRect().toRect();
    painter->drawImage(rect.topLeft(), imgPatch);
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
        auto pathLength = path.length();
        //QVector<QPointF> points;
        //for (double distance = 0; distance < pathLength; distance += mosaicRectSize) {
        //    double percent = distance / pathLength;
        //    points.append(path.pointAtPercent(percent));
        //}
        //drawRectsByPoints(points);
        initImgPatch();
    }
    auto win = (WinBase*)parent();
    win->update();
    event->accept();
}
