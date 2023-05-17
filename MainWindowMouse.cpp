#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMouseEvent>
#include <QCoreApplication>

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj->objectName() != "centralwidget")
    {
        return QObject::eventFilter(obj, event);
    }
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    bool flag = false;
    if (event->type() == QEvent::MouseMove)
    {
        flag = mouseMove(e);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        flag = mousePress(e);
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        flag = mouseRelease(e);
    }
    if (!flag)
    {
        return QObject::eventFilter(obj, event);
    }
    return true;
}

bool MainWindow::mousePress(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return true;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->pos();
        isMouseDown = true;
        if (state == "Start")
        {
            ui->toolMain->hide();
            return false;
        }
        else if (state == "maskReady")
        {
            ui->toolMain->hide();
            resizeMaskIndex = pointInMaskArea(mousePressPoint);
            resizeMask(mousePressPoint);
        }
        else if (state == "RectEllipse")
        {
            if (isMouseInDragger(mousePressPoint))
            {
                preState = state;
                state = "lastPathDrag";
                return true;
            }
            if (paths.count() > 0)
            {
                auto& path = paths.last();
                if (path.contains(mousePressPoint))
                {
                    preState = state;
                    state = "lastPathDrag";
                    draggerIndex = 8;
                    return true;
                }
            }
            QPainterPath path;
            path.moveTo(mousePressPoint);
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint);
            paths.append(path);
            return true;
        }
        else if (state == "Eraser")
        {
            QPainterPath path;
            path.moveTo(mousePressPoint);
            paths.append(path);
            painter1->setCompositionMode(QPainter::CompositionMode_DestinationOver);
            canvasImg1->fill(0);
            painter1->drawImage(0, 0, *canvasImg2);
            return true;
        }
    }
    return false;
}

bool MainWindow::mouseMove(QMouseEvent* mouseEvent)
{
    QPointF curPoint = mouseEvent->pos();
    if (isMouseDown)
    {
        if (state == "Start")
        {
            maskPath.setElementPositionAt(0, mousePressPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(2, curPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(4, mousePressPoint.x(), mousePressPoint.y());
            repaint();
            return true;
        }
        else if (state == "maskReady")
        {
            resizeMask(curPoint);
            return true;
        }
        else if (state == "RectEllipse")
        {
            auto& path = paths.last();
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
            painter1->setPen(QPen(QBrush(Qt::red), 2));
            painter1->setBrush(Qt::NoBrush);
            painter1->drawPath(path);
            repaint();
            return true;
        }
        else if (state == "lastPathDrag")
        {
            resizePath(curPoint);
            return true;
        }
        else if (state == "Eraser")
        {
            auto& path = paths.last();
            path.lineTo(curPoint.x(), curPoint.y());
            painter1->setRenderHint(QPainter::Antialiasing);
            painter1->setCompositionMode(QPainter::CompositionMode_Clear);
            painter1->setPen(QPen(QBrush(Qt::red), 12));
            painter1->setBrush(Qt::NoBrush);
            painter1->drawPath(path);
            repaint();
            return true;
        }
    }
    else
    {
        if (state == "maskReady")
        {
            int areaIndex = pointInMaskArea(curPoint);
            if (areaIndex == 0 || areaIndex == 4)
            {
                setCursor(Qt::SizeFDiagCursor);
                return true;
            }
            else if (areaIndex == 1 || areaIndex == 5)
            {
                setCursor(Qt::SizeVerCursor);
                return true;
            }
            else if (areaIndex == 2 || areaIndex == 6)
            {
                setCursor(Qt::SizeBDiagCursor);
                return true;
            }
            else if (areaIndex == 3 || areaIndex == 7)
            {
                setCursor(Qt::SizeHorCursor);
                return true;
            }
            else if (areaIndex == 8)
            {
                setCursor(Qt::SizeAllCursor);
                return true;
            }
        }
        else if (state == "RectEllipse")
        {
            if (paths.count() > 0)
            {
                if (isMouseInDragger(curPoint))
                {
                    switch (draggerIndex)
                    {
                        case 0:
                        case 4:
                            setCursor(Qt::SizeFDiagCursor);
                            break;
                        case 1:
                        case 5:
                            setCursor(Qt::SizeVerCursor);
                            break;
                        case 2:
                        case 6:
                            setCursor(Qt::SizeBDiagCursor);
                            break;
                        case 3:
                        case 7:
                            setCursor(Qt::SizeHorCursor);
                            break;
                    }
                    return true;
                }
                else
                {
                    auto& path = paths.last();
                    if (path.contains(curPoint))
                    {
                        setCursor(Qt::SizeAllCursor);
                    }
                    else
                    {
                        setCursor(Qt::CrossCursor);
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool MainWindow::mouseRelease(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        isMouseDown = false;
        if (state == "Start" || state == "maskReady")
        {
            resetPathPoint(maskPath);
            showToolMain();
            state = "maskReady";
            return true;
        }
        else if (state == "RectEllipse")
        {
            auto& path = paths.last();
            resetPathPoint(path);
            repaint();
            ui->btnUndo->setStyleSheet("");
            setDraggerPosition(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
            setCursor(Qt::CrossCursor);
            repaint();
            //            painter2->setPen(QPen(QBrush(Qt::red), 2));
            //            painter2->setBrush(Qt::NoBrush);
            //            painter2->drawPath(path);
            return true;
        }
        else if (state == "lastPathDrag")
        {
            auto& path = paths.last();
            resetPathPoint(path);
            state = preState;
            return true;
        }
    }
    return false;
}
