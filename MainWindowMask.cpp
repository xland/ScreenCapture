#include "MainWindow.h"
#include "ScreenShoter.h"


void MainWindow::initMask()
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

int MainWindow::pointInMaskArea(const QPointF& curPoint)
{
    qreal left = maskPath.elementAt(5).x;
    qreal top = maskPath.elementAt(5).y;
    qreal right = maskPath.elementAt(7).x;
    qreal bottom = maskPath.elementAt(7).y;
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
        maskPath.setElementPositionAt(5, point.x(), point.y());
        maskPath.setElementPositionAt(6, maskPath.elementAt(6).x, point.y());
        maskPath.setElementPositionAt(9, point.x(), point.y());
        maskPath.setElementPositionAt(8, point.x(), maskPath.elementAt(8).y);
        repaint();
    }
    else if (resizeMaskIndex == 1)
    {
        maskPath.setElementPositionAt(5, maskPath.elementAt(5).x, point.y());
        maskPath.setElementPositionAt(6, maskPath.elementAt(6).x, point.y());
        maskPath.setElementPositionAt(9, maskPath.elementAt(9).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 2)
    {
        maskPath.setElementPositionAt(5, maskPath.elementAt(5).x, point.y());
        maskPath.setElementPositionAt(6, point.x(), point.y());
        maskPath.setElementPositionAt(7, point.x(), maskPath.elementAt(7).y);
        maskPath.setElementPositionAt(9, maskPath.elementAt(9).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 3)
    {
        maskPath.setElementPositionAt(6, point.x(), maskPath.elementAt(6).y);
        maskPath.setElementPositionAt(7, point.x(), maskPath.elementAt(7).y);
        repaint();
    }
    else if (resizeMaskIndex == 4)
    {
        maskPath.setElementPositionAt(6, point.x(), maskPath.elementAt(6).y);
        maskPath.setElementPositionAt(7, point.x(), point.y());
        maskPath.setElementPositionAt(8, maskPath.elementAt(8).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 5)
    {
        maskPath.setElementPositionAt(7, maskPath.elementAt(7).x, point.y());
        maskPath.setElementPositionAt(8, maskPath.elementAt(8).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 6)
    {
        maskPath.setElementPositionAt(7, maskPath.elementAt(7).x, point.y());
        maskPath.setElementPositionAt(8, point.x(), point.y());
        maskPath.setElementPositionAt(9, point.x(), maskPath.elementAt(9).y);
        maskPath.setElementPositionAt(5, point.x(), maskPath.elementAt(5).y);
        repaint();
    }
    else if (resizeMaskIndex == 7)
    {
        maskPath.setElementPositionAt(8, point.x(), maskPath.elementAt(8).y);
        maskPath.setElementPositionAt(9, point.x(), maskPath.elementAt(9).y);
        maskPath.setElementPositionAt(5, point.x(), maskPath.elementAt(5).y);
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
            maskPath.setElementPositionAt(5, maskPath.elementAt(5).x + xSpan, maskPath.elementAt(5).y + ySpan);
            maskPath.setElementPositionAt(6, maskPath.elementAt(6).x + xSpan, maskPath.elementAt(6).y + ySpan);
            maskPath.setElementPositionAt(7, maskPath.elementAt(7).x + xSpan, maskPath.elementAt(7).y + ySpan);
            maskPath.setElementPositionAt(8, maskPath.elementAt(8).x + xSpan, maskPath.elementAt(8).y + ySpan);
            maskPath.setElementPositionAt(9, maskPath.elementAt(9).x + xSpan, maskPath.elementAt(9).y + ySpan);
            mousePressPoint = point;
            repaint();
        }
    }
}
