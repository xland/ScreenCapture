#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QLineEdit>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QTextFrame>
#include "ScreenShoter.h"

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseMove && !isMouseDown && state == "Start")
    {
        moveTipBox();
    }
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
    ui->tipBox->hide();
    if (mouseEvent->button() == Qt::RightButton)
    {
        if (textInputBox->isVisible())
        {
            //todo
            auto& path = createPath();
            path.isText = true;
            path.text = textInputBox->toPlainText();
            path.textRect = textInputBox->geometry();
            path.textRect.moveTo(path.textRect.x() + 5, path.textRect.y() + 5);
            path.color = colorSelector->currentColor();
            path.textFont = textInputBox->font();
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            textInputBox->clear();
            textInputBox->hide();
            repaint();
            return true;
        }
        if(showDraggerCount > 0){
            showDraggerCount = 0;
            repaint();
            return true;
        }
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
            return true;
        }
        else if (state == "RectEllipse")
        {
            if (isMouseInDragger(mousePressPoint))
            {
                preState = state;
                state = "lastPathDrag";
                return true;
            }
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotRectEllipse->size;
            path.needFill = ui->btnRectEllipseFill->isChecked();
            path.isEllipse = ui->btnEllipse->isChecked();
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
            return true;
        }
        else if (state == "Pen")
        {
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotPen->size;
            path.needFill = false;
            path.moveTo(mousePressPoint);
            return true;
        }
        else if (state == "Mosaic")
        {
            endOneDraw();
            auto& path = createPath();
            path.borderWidth = dotMosaic->size;
            path.needFill = false;
            path.isMosaic = true;
            path.moveTo(mousePressPoint);
            return true;
        }
        else if (state == "Text")
        {
            if (draggingTextState == 1)
            {
                return true;
            }
            endOneDraw();
            textInputBox->move(mousePressPoint.x() - textInputBox->width() / 2, mousePressPoint.y() - textInputBox->height() / 2);
            textInputBox->show();
            textInputBox->setFocus(Qt::OtherFocusReason);
            return true; 
        }
        else if (state == "Eraser")
        {
            endOneDraw();
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            layerBgPainter->drawImage(0, 0, ScreenShoter::Get()->desktopImages[0]);
            isDrawing = true;
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
            drawArrow(curPoint);
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
            isDrawing = true;
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
        else if (state == "Text")
        {
            if (draggingTextState == 1)
            {
                qreal xSpan = curPoint.x() - mousePressPoint.x();
                qreal ySpan = curPoint.y() - mousePressPoint.y();
                auto& path = paths.last();
                path.textRect.moveTo(path.textRect.topLeft().x() + xSpan, path.textRect.topLeft().y() + ySpan);
                layerDrawingImg->fill(0);
                paintPath(path, layerDrawingPainter);
                isDrawing = true;
                repaint();
                mousePressPoint = curPoint;
            }
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
                        case 8:
                            setCursor(Qt::SizeAllCursor);
                            break;
                    }
                    return true;
                }
                else
                {
                    setCursor(Qt::CrossCursor);
                    return true;
                }
            }
        }
        else if (state == "Text")
        {
            if (paths.count() > 0)
            {
                auto& path = paths.last();
                if (path.isText && path.textRect.contains(curPoint.toPoint()))
                {
                    setCursor(Qt::SizeAllCursor);
                    draggingTextState = 1;
                    return true;
                }
            }
            draggingTextState = 0;
            setCursor(Qt::IBeamCursor);
            return true;
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
            setDraggerPosition(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
            ui->btnUndo->setStyleSheet("");
            setCursor(Qt::CrossCursor);
            repaint();
            return true;
        }
        else if (state == "Arrow")
        {
            ui->btnUndo->setStyleSheet("");
            return true;
        }
        else if (state == "Pen")
        {
            ui->btnUndo->setStyleSheet("");
            return true;
        }
        else if (state == "Eraser")
        {
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
            ui->btnUndo->setStyleSheet("");
            repaint();
            return true;
        }
        else if (state == "Mosaic")
        {
            ui->btnUndo->setStyleSheet("");
            repaint();
            return true;
        }
        else if (state == "lastPathDrag")
        {
            auto& path = paths.last();
            path.resetPoint5();
            setDraggerPosition(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
            state = preState;
            return true;
        }
    }
    return false;
}
