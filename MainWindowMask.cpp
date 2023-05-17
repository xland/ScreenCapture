#include "MainWindow.h"
#include "ScreenShoter.h"


void MainWindow::initMask()
{
    maskPath.borderColor = QColor(22, 119, 255);
    maskPath.fillColor = QColor(0, 0, 0, 120);
    maskPath.needFill = true;

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

int MainWindow::pointInMaskArea(const QPointF& curPoint)
{
    qreal left = maskPath.elementAt(0).x;
    qreal top = maskPath.elementAt(0).y;
    qreal right = maskPath.elementAt(2).x;
    qreal bottom = maskPath.elementAt(2).y;
    if (curPoint.x() < left && curPoint.y() < top)
    {
        return 0;
    }
    else if (curPoint.x() > left && curPoint.y() < top && curPoint.x() < right)
    {
        return 1;
    }
    else if (curPoint.x() > right && curPoint.y() < top)
    {
        return 2;
    }
    else if (curPoint.x() > right && curPoint.y() > top && curPoint.y() < bottom)
    {
        return 3;
    }
    else if (curPoint.x() > right  && curPoint.y() > bottom)
    {
        return 4;
    }
    else if (curPoint.x() > left && curPoint.x() < right && curPoint.y() > bottom)
    {
        return 5;
    }
    else if (curPoint.x() < left  && curPoint.y() > bottom)
    {
        return 6;
    }
    else if (curPoint.x() < left  && curPoint.y() > top && curPoint.y() < bottom)
    {
        return 7;
    }
    else
    {
        return 8;
    }
}

void MainWindow::resizeMask(const QPointF& point)
{
    if (resizeMaskIndex == 0)
    {
        maskPath.setElementPositionAt(0, point.x(), point.y());
        maskPath.setElementPositionAt(1, maskPath.elementAt(1).x, point.y());
        maskPath.setElementPositionAt(4, point.x(), point.y());
        maskPath.setElementPositionAt(3, point.x(), maskPath.elementAt(3).y);
        repaint();
    }
    else if (resizeMaskIndex == 1)
    {
        maskPath.setElementPositionAt(0, maskPath.elementAt(0).x, point.y());
        maskPath.setElementPositionAt(1, maskPath.elementAt(1).x, point.y());
        maskPath.setElementPositionAt(4, maskPath.elementAt(4).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 2)
    {
        maskPath.setElementPositionAt(0, maskPath.elementAt(0).x, point.y());
        maskPath.setElementPositionAt(1, point.x(), point.y());
        maskPath.setElementPositionAt(2, point.x(), maskPath.elementAt(2).y);
        maskPath.setElementPositionAt(4, maskPath.elementAt(4).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 3)
    {
        maskPath.setElementPositionAt(1, point.x(), maskPath.elementAt(1).y);
        maskPath.setElementPositionAt(2, point.x(), maskPath.elementAt(2).y);
        repaint();
    }
    else if (resizeMaskIndex == 4)
    {
        maskPath.setElementPositionAt(1, point.x(), maskPath.elementAt(1).y);
        maskPath.setElementPositionAt(2, point.x(), point.y());
        maskPath.setElementPositionAt(3, maskPath.elementAt(3).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 5)
    {
        maskPath.setElementPositionAt(2, maskPath.elementAt(2).x, point.y());
        maskPath.setElementPositionAt(3, maskPath.elementAt(3).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 6)
    {
        maskPath.setElementPositionAt(2, maskPath.elementAt(2).x, point.y());
        maskPath.setElementPositionAt(3, point.x(), point.y());
        maskPath.setElementPositionAt(4, point.x(), maskPath.elementAt(4).y);
        maskPath.setElementPositionAt(0, point.x(), maskPath.elementAt(0).y);
        repaint();
    }
    else if (resizeMaskIndex == 7)
    {
        maskPath.setElementPositionAt(3, point.x(), maskPath.elementAt(3).y);
        maskPath.setElementPositionAt(4, point.x(), maskPath.elementAt(4).y);
        maskPath.setElementPositionAt(0, point.x(), maskPath.elementAt(0).y);
        repaint();
    }
    else if (resizeMaskIndex == 8)
    {
        if (point == mousePressPoint)
        {
            return;
        }
        else
        {
            qreal xSpan = point.x() - mousePressPoint.x();
            qreal ySpan = point.y() - mousePressPoint.y();
            maskPath.setElementPositionAt(0, maskPath.elementAt(0).x + xSpan, maskPath.elementAt(0).y + ySpan);
            maskPath.setElementPositionAt(1, maskPath.elementAt(1).x + xSpan, maskPath.elementAt(1).y + ySpan);
            maskPath.setElementPositionAt(2, maskPath.elementAt(2).x + xSpan, maskPath.elementAt(2).y + ySpan);
            maskPath.setElementPositionAt(3, maskPath.elementAt(3).x + xSpan, maskPath.elementAt(3).y + ySpan);
            maskPath.setElementPositionAt(4, maskPath.elementAt(4).x + xSpan, maskPath.elementAt(4).y + ySpan);
            mousePressPoint = point;
            repaint();
        }
    }
}
