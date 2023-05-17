#include "MainWindow.h"


void MainWindow::initDragger()
{
    for (int var = 0; var < 8; ++var)
    {
        dragers.append(QRectF(0, 0, 8, 8));
    }
}

void MainWindow::draggerPressed(int draggerIndex)
{
    state = "lastPathDrag";
    isMouseDown = true;
    this->draggerIndex = draggerIndex;
    mousePressPoint = QCursor::pos();
}

void MainWindow::resizePath(const QPointF& point)
{
    auto& path = paths.last();
    if (draggerIndex == 0)
    {
        path.setElementPositionAt(0, point.x(), point.y());
        path.setElementPositionAt(1, path.elementAt(1).x, point.y());
        path.setElementPositionAt(3, point.x(), path.elementAt(3).y);
        path.setElementPositionAt(4, point.x(), point.y());
    }
    else if (draggerIndex == 1)
    {
        path.setElementPositionAt(0, path.elementAt(0).x, point.y());
        path.setElementPositionAt(1, path.elementAt(1).x, point.y());
        path.setElementPositionAt(4, path.elementAt(4).x, point.y());
    }
    else if (draggerIndex == 2)
    {
        path.setElementPositionAt(0, path.elementAt(0).x, point.y());
        path.setElementPositionAt(1, point.x(), point.y());
        path.setElementPositionAt(2, point.x(), path.elementAt(2).y);
        path.setElementPositionAt(4, path.elementAt(4).x, point.y());
    }
    else if (draggerIndex == 3)
    {
        path.setElementPositionAt(1, point.x(), path.elementAt(1).y);
        path.setElementPositionAt(2, point.x(), path.elementAt(2).y);
    }
    else if (draggerIndex == 4)
    {
        path.setElementPositionAt(1, point.x(), path.elementAt(1).y);
        path.setElementPositionAt(2, point.x(), point.y());
        path.setElementPositionAt(3, path.elementAt(3).x, point.y());
    }
    else if (draggerIndex == 5)
    {
        path.setElementPositionAt(2, path.elementAt(2).x, point.y());
        path.setElementPositionAt(3, path.elementAt(3).x, point.y());
    }
    else if (draggerIndex == 6)
    {
        path.setElementPositionAt(2, path.elementAt(2).x, point.y());
        path.setElementPositionAt(3, point.x(), point.y());
        path.setElementPositionAt(4, point.x(), path.elementAt(4).y);
        path.setElementPositionAt(0, point.x(), path.elementAt(0).y);
    }
    else if (draggerIndex == 7)
    {
        path.setElementPositionAt(3, point.x(), path.elementAt(3).y);
        path.setElementPositionAt(4, point.x(), path.elementAt(4).y);
        path.setElementPositionAt(0, point.x(), path.elementAt(0).y);
    }
    else if (draggerIndex == 8)
    {
        if (point == mousePressPoint)
        {
            return;
        }
        else
        {
            qreal xSpan = point.x() - mousePressPoint.x();
            qreal ySpan = point.y() - mousePressPoint.y();
            maskPath.setElementPositionAt(5, maskPath.elementAt(5).x + xSpan, maskPath.elementAt(5).y + ySpan);
            maskPath.setElementPositionAt(6, maskPath.elementAt(6).x + xSpan, maskPath.elementAt(6).y + ySpan);
            maskPath.setElementPositionAt(7, maskPath.elementAt(7).x + xSpan, maskPath.elementAt(7).y + ySpan);
            maskPath.setElementPositionAt(8, maskPath.elementAt(8).x + xSpan, maskPath.elementAt(8).y + ySpan);
            maskPath.setElementPositionAt(9, maskPath.elementAt(9).x + xSpan, maskPath.elementAt(9).y + ySpan);
            mousePressPoint = point;
        }
    }
    showDragger(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    painter1->setPen(QPen(QBrush(Qt::red), 2));
    painter1->setBrush(Qt::NoBrush);
    painter1->drawPath(path);
    repaint();
}

bool MainWindow::isMouseInDragger(const QPointF& point)
{
    for (int var = 0; var < showDraggerCount; ++var)
    {
        if (dragers[var].contains(point))
        {
            draggerIndex = var;
            return true;
        }
    }
    return false;
}

void MainWindow::showDragger(qreal x1, qreal y1, qreal x2, qreal y2)
{
    dragers[0].moveTo(x1 - 4, y1 - 4);
    dragers[1].moveTo(x1 + (x2 - x1) / 2 - 4, y1 - 4);
    dragers[2].moveTo(x2 - 4, y1 - 4);
    dragers[3].moveTo(x2 - 4, y1 + (y2 - y1) / 2 - 4);
    dragers[4].moveTo(x2 - 4, y2 - 4);
    dragers[5].moveTo(x1 + (x2 - x1) / 2 - 4, y2 - 4);
    dragers[6].moveTo(x1 - 4, y2 - 4);
    dragers[7].moveTo(x1 - 4, y1 + (y2 - y1) / 2 - 4);
    showDraggerCount = 8;
}
