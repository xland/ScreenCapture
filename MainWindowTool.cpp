#include "MainWindow.h"


void MainWindow::resetPathPoint(QPainterPath& path)
{
    qreal x2 = -999.0, x1 = 999999999.0;
    qreal y2 = -999.0, y1 = 999999999.0;
    for (int var = 0; var < 5; ++var)
    {
        auto ele = path.elementAt(var);
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
    path.setElementPositionAt(0, x1, y1);
    path.setElementPositionAt(1, x2, y1);
    path.setElementPositionAt(2, x2, y2);
    path.setElementPositionAt(3, x1, y2);
    path.setElementPositionAt(4, x1, y1);
}
