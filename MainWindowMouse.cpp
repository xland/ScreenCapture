#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QLineEdit>
#include <QMouseEvent>
#include <QCoreApplication>
#include "ScreenShoter.h"

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
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotRectEllipse->size;
            path.needFill = ui->btnRectEllipseFill->isChecked();
            path.moveTo(mousePressPoint);
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint);
            return true;
        }
        else if (state == "Arrow")
        {
            endOneDraw();
            auto& path = createPath();
            path.needFill = ui->btnArrowFill->isChecked();
            path.moveTo(mousePressPoint);
            path.lineTo(QPointF(0, 0));
            path.lineTo(QPointF(1, 1));
            path.lineTo(QPointF(2, 2));
            path.lineTo(QPointF(3, 3));
            path.lineTo(QPointF(4, 4));
            path.lineTo(mousePressPoint);
        }
        else if (state == "Pen")
        {
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotPen->size;
            path.needFill = false;
            path.moveTo(mousePressPoint);
        }
        else if (state == "Mosaic")
        {
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotMosaic->size;
            path.needFill = false;
            path.isMosaic = true;
            path.moveTo(mousePressPoint);
        }
        else if (state == "Text")
        {
            ui->lineEdit->move(mousePressPoint.x() - ui->lineEdit->width() / 2, mousePressPoint.y() - ui->lineEdit->height() / 2);
            ui->lineEdit->show();
            ui->lineEdit->setFocus(Qt::OtherFocusReason);
        }
        else if (state == "Eraser")
        {
            endOneDraw();
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
            auto& path = createPath();
            path.borderWidth = dotEraser->size;
            path.needFill = false;
            path.isEraser = true;
            path.borderWidth = 24;
            path.moveTo(mousePressPoint);
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
            path.needDelete = false;
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            layerDrawingImg->fill(0);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "lastPathDrag")
        {
            resizePath(curPoint);
            auto& path = paths.last();
            layerDrawingImg->fill(0);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "Arrow")
        {
            qreal height = 26.0, width = 22.0;
            auto& path = paths.last();
            //箭头起点和箭头终点组成的线段，这个线段与水平坐标的夹角
            QLineF tarLine(mousePressPoint, curPoint);
            //角度变弧度，以适应std::cos，std::sin
            qreal v = tarLine.angle() * 3.14159265 / 180;
            // △底边的中点
            qreal centerX = curPoint.x() - height * std::cos(v);
            qreal centerY = curPoint.y() + height * std::sin(v);
            qreal tempA = width / 4 * std::sin(v);
            qreal tempB = width / 4 * std::cos(v);
            // △ 左下的顶点与底边中点之间中间位置的点
            qreal x1 = centerX - tempA;
            qreal y1 = centerY - tempB;
            path.setElementPositionAt(1, x1, y1);
            // △ 左下的顶点
            qreal x2 = x1 - tempA;
            qreal y2 = y1 - tempB;
            path.setElementPositionAt(2, x2, y2);
            // △ 上部顶点，也就是箭头终点
            path.setElementPositionAt(3, curPoint.x(), curPoint.y());
            // △ 右下顶点
            tempA = width / 2 * std::sin(v);
            tempB = width / 2 * std::cos(v);
            qreal x3 = centerX + tempA;
            qreal y3 = centerY + tempB;
            path.setElementPositionAt(4, x3, y3);
            // △ 右下的顶点与底边中点之间中间位置的点
            qreal x4 = centerX + tempA / 2;
            qreal y4 = centerY + tempB / 2;
            path.setElementPositionAt(5, x4, y4);
            layerDrawingImg->fill(0);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "Pen")
        {
            auto& path = paths.last();
            path.lineTo(curPoint);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "Mosaic")
        {
            auto& path = paths.last();
            path.lineTo(curPoint);
            paintPath(path, layerBgPainter);
            isDrawing = false;
            repaint();
            return true;
        }
        else if (state == "Eraser")
        {
            auto& path = paths.last();
            path.lineTo(curPoint);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
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
        else if (state == "Text")
        {
            setCursor(Qt::IBeamCursor);
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
            maskPath.resetPoint5();
            showToolMain();
            state = "maskReady";
            return true;
        }
        else if (state == "RectEllipse")
        {
            auto& path = paths.last();
            if (path.needDelete)
            {
                paths.removeLast();
                return true;
            }
            path.resetPoint5();
            ui->btnUndo->setStyleSheet("");
            setDraggerPosition(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
            setCursor(Qt::CrossCursor);
            repaint();
            return true;
        }
        else if (state == "Arrow")
        {
            ui->btnUndo->setStyleSheet("");
        }
        else if (state == "Pen")
        {
            ui->btnUndo->setStyleSheet("");
        }
        else if (state == "Eraser")
        {
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
            ui->btnUndo->setStyleSheet("");
            repaint();
        }
        else if (state == "Mosaic")
        {
            ui->btnUndo->setStyleSheet("");
            repaint();
        }
        else if (state == "lastPathDrag")
        {
            auto& path = paths.last();
            path.resetPoint5();
            state = preState;
            return true;
        }
    }
    return false;
}
