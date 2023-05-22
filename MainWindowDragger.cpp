#include "MainWindow.h"


void MainWindow::initDragger()
{
    for (int var = 0; var < 8; ++var)
    {
        dragers.append(QRectF(0, 0, 8, 8));
    }
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
            path.setElementPositionAt(0, path.elementAt(0).x + xSpan, path.elementAt(0).y + ySpan);
            path.setElementPositionAt(1, path.elementAt(1).x + xSpan, path.elementAt(1).y + ySpan);
            path.setElementPositionAt(2, path.elementAt(2).x + xSpan, path.elementAt(2).y + ySpan);
            path.setElementPositionAt(3, path.elementAt(3).x + xSpan, path.elementAt(3).y + ySpan);
            path.setElementPositionAt(4, path.elementAt(4).x + xSpan, path.elementAt(4).y + ySpan);
            mousePressPoint = point;
        }
    }
    setDraggerPosition(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);

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

void MainWindow::setDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2)
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
