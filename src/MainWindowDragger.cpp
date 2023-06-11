#include "MainWindow.h"
#include <QDebug>


void MainWindow::initDragger()
{
    for (int var = 0; var < 8; ++var)
    {
        dragers.append(QRectF(0, 0, 8, 8));
    }
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
    if (paths.count() > 0)
    {
        auto& path = paths.last();
        if (path->needPaint && path->contains(point))
        {
            draggerIndex = 8;
            return true;
        }
    }
    return false;
}

void MainWindow::setDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2)
{
    x1 -= 4;
    y1 -= 4;
    x2 -= 4;
    y2 -= 4;
    dragers[0].moveTo(x1, y1);
    dragers[1].moveTo(x1 + (x2 - x1) / 2, y1);
    dragers[2].moveTo(x2, y1);
    dragers[3].moveTo(x2, y1 + (y2 - y1) / 2);
    dragers[4].moveTo(x2, y2);
    dragers[5].moveTo(x1 + (x2 - x1) / 2, y2);
    dragers[6].moveTo(x1, y2);
    dragers[7].moveTo(x1, y1 + (y2 - y1) / 2);
    showDraggerCount = 8;
}
