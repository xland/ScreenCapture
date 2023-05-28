#include "PathModel.h"
#include <QPainter>
#include <QDateTime>
#include <QDebug>

PathModel::PathModel()
{

}

PathModel::~PathModel()
{
    qDebug() << "release path";
}

void PathModel::resetPoint5()
{
    qreal x2 = -999.0, x1 = 999999999.0;
    qreal y2 = -999.0, y1 = 999999999.0;
    for (int var = 0; var < 5; ++var)
    {
        auto ele = elementAt(var);
        if (ele.x < x1)
        {
            x1 = ele.x;
        }
        if (ele.x > x2)
        {
            x2 = ele.x;
        }
        if (ele.y < y1)
        {
            y1 = ele.y;
        }
        if (ele.y > y2)
        {
            y2 = ele.y;
        }
    }
    setElementPositionAt(0, x1, y1);
    setElementPositionAt(1, x2, y1);
    setElementPositionAt(2, x2, y2);
    setElementPositionAt(3, x1, y2);
    setElementPositionAt(4, x1, y1);

//    auto rect = boundingRect();
//    setElementPositionAt(0, rect.topLeft().x(), rect.topLeft().y());
//    setElementPositionAt(1, rect.topRight().x(), rect.topRight().y());
//    setElementPositionAt(2, rect.bottomRight().x(), rect.bottomRight().y());
//    setElementPositionAt(3, rect.bottomLeft().x(), rect.bottomLeft().y());
//    setElementPositionAt(4, rect.topLeft().x(), rect.topLeft().y());
}

void PathModel::initPatch(QImage* bgImg, qreal scaleFactor)
{
    if (patchImg.isNull())
    {
        auto rect = controlPointRect().toRect();
        patchPosition  = rect.topLeft();
        patchPosition.setX(patchPosition.x() - borderWidth);
        patchPosition.setY(patchPosition.y() - borderWidth);
        auto point = patchPosition * scaleFactor;
        rect.moveTo(point);
        rect.setWidth(rect.width() + borderWidth * 2);
        rect.setHeight(rect.height() + borderWidth * 2);
        rect.setSize(rect.size() * scaleFactor);
        patchImg =  bgImg->copy(rect); //todo 定义时的初始化，浪费资源
    }
}
