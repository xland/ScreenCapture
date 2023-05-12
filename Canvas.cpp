
#include "Canvas.h"
#include <QColor>
#include <QBrush>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QPen>
#include <QImage>
#include "MainWindow.h"
#include "ScreenShoter.h"

Canvas::Canvas(QWidget* parent)
    : QWidget{parent}
{
    initMask();
    auto screenShoter = ScreenShoter::Get();
    canvasImg = new QImage(screenShoter->screenRects[0].size(), QImage::Format_ARGB32_Premultiplied);
    canvasImg->fill(0);
    painter = new QPainter(canvasImg);
}
Canvas::~Canvas()
{
    delete painter;
    delete canvasImg;
}

void Canvas::paintEvent(QPaintEvent* e)
{
    painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter->setPen(Qt::NoPen);
    painter->fillRect(QRect(100, 100, 100, 100), QBrush(Qt::black));

    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->fillRect(QRect(150, 150, 100, 100), QBrush(Qt::black));


    QPainter p1(this);
    p1.drawImage(0, 0, *canvasImg);
    p1.end();

//    QPainter painter(this);
//    painter.setPen(QPen(Qt::red, 13));
//    painter.drawRect(QRect(50, 50, 200, 200));
//    painter.save();

//    QPainterPath path;
//    path.addRect(QRect(100, 100, 200, 200));
//    painter.setPen(QPen(Qt::black, 13));
//    painter.setClipPath(path, Qt::IntersectClip);

//    painter.drawPath(path);
//    painter.end();

    //如果做到一个path里，那么就没办法让橡皮擦的路径遮盖其他的路径
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    QPen pen(QBrush(QColor(22, 119, 255)), maskBorderWidth);
//    QBrush brush(QColor(0, 0, 0, 120));
//    painter.setPen(pen);
//    painter.setBrush(brush);
//    painter.drawPath(maskPath);

//    pen.setColor(Qt::red);
//    brush.setColor(Qt::black);
//    for (int var = 0; var < paths.count(); ++var)
//    {
//        painter.setPen(pen);
//        painter.setBrush(brush);
//        if (var > 0)
//        {
//            painter.setCompositionMode(QPainter::CompositionMode_Clear);
//        }

//        painter.drawPath(paths[var]);
//    }
}

void Canvas::initMask()
{
    auto screenShoter = ScreenShoter::Get();
    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);

    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);
}

void Canvas::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->pos();
        isMouseDown = true;
        if (state == State::start)
        {
        }
        else if (state == State::rect)
        {
            QPainterPath path;
            path.moveTo(mousePressPoint);
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint);
            paths.append(path);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (isMouseDown)
    {
        auto curPoint = mouseEvent->pos();
        if (state == State::start)
        {
            maskPath.setElementPositionAt(5, mousePressPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(6, curPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(7, curPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(8, mousePressPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(9, mousePressPoint.x(), mousePressPoint.y());
            repaint();
        }
        else if (state == State::rect)
        {
            auto& path = paths.last();
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            repaint();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
//    if (mouseEvent->button() == Qt::LeftButton)
//    {
//        isMouseDown = false;
//        auto mainWin = (MainWindow*)(qApp->activeWindow());
//        //todo 这个位置要动态的，工具条应该出现在正确的位置上
//        mainWin->showToolMain(maskPath.elementAt(7).x, maskPath.elementAt(7).y);
//    }
}
