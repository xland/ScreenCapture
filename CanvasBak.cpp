
#include "CanvasBak.h"
#include "ScreenShoter.h"
#include <QPainterPath>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QColor>
#include <QDebug>
#include "MainWindow.h"

CanvasBak::CanvasBak(QObject* parent)
    : QGraphicsScene{parent}
{
    auto screenShoter = ScreenShoter::Get();
    auto view = static_cast<QGraphicsView*>(parent);
    view->setGeometry(screenShoter->screenRects[0]);
    setSceneRect(screenShoter->screenRects[0]);
    //todo 多屏支持
    auto imgItem = addPixmap(screenShoter->desktopImages[0]);
    imgItem->setPos(0, 0);
    initMask();

}
CanvasBak::~CanvasBak()
{

}
void CanvasBak::initMask()
{
    auto screenShoter = ScreenShoter::Get();
    QPainterPath maskPath;
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

    QPen pen(QBrush(QColor(22, 119, 255)), maskBorderWidth);
    maskPathItem = addPath(maskPath, pen, QBrush(QColor(0, 0, 0, 120)));
    maskPathItem->setZValue(999);
//    maskPathItem->setCacheMode(QGraphicsItem::ItemCoordinateCache);


    QPainterPath p1;
    p1.moveTo(0, 0);
    p1.lineTo(600, 600);

    p1.moveTo(0, 600);
    p1.lineTo(600, 0);
    addPath(p1, pen);
}

void CanvasBak::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->scenePos();
        isMouseDown = true;
        if (state == State::start)
        {
        }
        else if (state == State::rect)
        {
            QPainterPath rectPath;
            rectPath.moveTo(mousePressPoint);
            rectPath.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            rectPath.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            rectPath.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            rectPath.lineTo(mousePressPoint);
            QPen pen(QBrush(Qt::red), 2);
            test = false;
            rectPathItem = addPath(rectPath, pen, QBrush(Qt::transparent));
//            rectPathItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        }
    }
}

void CanvasBak::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (isMouseDown)
    {
        auto curPoint = mouseEvent->scenePos();
        if (state == State::start)
        {
            auto path = maskPathItem->path();
            path.setElementPositionAt(5, mousePressPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(6, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(7, curPoint.x(), curPoint.y());
            path.setElementPositionAt(8, mousePressPoint.x(), curPoint.y());
            path.setElementPositionAt(9, mousePressPoint.x(), mousePressPoint.y());
            maskPathItem->setPath(path);
            update();
        }
        else if (state == State::rect)
        {
            auto path = rectPathItem->path();
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            rectPathItem->setPath(path);
            update();
        }
    }
}

void CanvasBak::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
//    if (mouseEvent->button() == Qt::LeftButton)
//    {
//        isMouseDown = false;
//        auto path = maskPathItem->path();
//        //todo 这个位置要动态的，工具条应该出现在正确的位置上
//        showToolMain(path.elementAt(7).x, path.elementAt(7).y);
//    }
}

