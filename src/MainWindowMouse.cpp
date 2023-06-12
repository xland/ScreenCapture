#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QLineEdit>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QTextFrame>
#include <QScreen>

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    if (event->type() == QEvent::MouseMove && !isMouseDown && state == "Start")
    {        
        moveTipBox();
        highlightWindow();
    }
    if (obj->objectName() != "centralwidget")
    {
        return QObject::eventFilter(obj, event);
    }
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
    mousePressPoint = getNativeMousePos();
    if (mouseEvent->button() == Qt::RightButton)
    {
        if (this->textInputBox->isVisible())
        {
            translateTextToPath();
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
            removeUndoPath();
            endOneDraw();
            auto path = createPath();
            path->borderWidth = dotRectEllipse->size;
            path->needFill = ui->btnRectEllipseFill->isChecked();
            path->isEllipse = ui->btnEllipse->isChecked();
            path->moveTo(mousePressPoint);
            path->lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path->lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path->lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path->lineTo(mousePressPoint);
            return true;
        }
        else if (state == "Arrow")
        {
            removeUndoPath();
            endOneDraw();
            auto path = createPath();
            path->needFill = ui->btnArrowFill->isChecked();
            path->moveTo(mousePressPoint);
            path->lineTo(QPointF(0, 0));
            path->lineTo(QPointF(1, 1));
            path->lineTo(QPointF(2, 2));
            path->lineTo(QPointF(3, 3));
            path->lineTo(QPointF(4, 4));
            path->lineTo(mousePressPoint);
            return true;
        }
        else if (state == "Pen")
        {
            removeUndoPath();
            endOneDraw();
            auto path = createPath();
            path->borderWidth = dotPen->size;
            path->needFill = false;
            path->moveTo(mousePressPoint);
            return true;
        }
        else if (state == "Mosaic")
        {
            removeUndoPath();
            endOneDraw();
            auto path = createPath();
            path->borderWidth = dotMosaic->size;
            path->needFill = false;
            path->isMosaic = true;
            path->moveTo(mousePressPoint);
            return true;
        }
        else if (state == "Text")
        {
            if (draggingTextState == 1)
            {
                return true;
            }
            removeUndoPath();
            endOneDraw();
            textInputBox->move(mousePressPoint.x() - textInputBox->width() / 2, mousePressPoint.y() - textInputBox->height() / 2);
            textInputBox->show();
            textInputBox->setFocus(Qt::OtherFocusReason);
            return true; 
        }
        else if (state == "Eraser")
        {
            removeUndoPath();
            endOneDraw();
            //��bgͼ�㿽����drawingͼ��
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            //������ͼ�񿽱���bgͼ�㣬��Ƥ�����οյ�·��������drawingͼ���ϣ���������ȥ���ǰ���ǰ����·��������
            //memcpy(layerBgImg->bits(), desktopImage->bits(), layerBgImg->sizeInBytes());
            layerBgPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            layerBgPainter->drawImage(0, 0, *desktopImage);
            isDrawing = true;
            auto path = createPath();
            path->borderWidth = dotEraser->size;
            path->needFill = false;
            path->isEraser = true;
            path->borderWidth = 24;
            path->moveTo(mousePressPoint);
            return true;
        }
    }
    return false;
}

bool MainWindow::mouseMove(QMouseEvent* mouseEvent)
{
    QPointF curPoint = getNativeMousePos();
    if (isMouseDown)
    {
        if (state == "Start")
        {
            setMask(mousePressPoint, curPoint);
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
            path->needDelete = false;
            path->setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path->setElementPositionAt(2, curPoint.x(), curPoint.y());
            path->setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
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
            auto path = paths.last();
            path->lineTo(curPoint);
            paintPath(path, layerDrawingPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "Mosaic")
        {
            auto path = paths.last();
            path->lineTo(curPoint);
            paintPath(path, layerBgPainter);
            isDrawing = true;
            repaint();
            return true;
        }
        else if (state == "Eraser")
        {
            auto path = paths.last();
            path->lineTo(curPoint);
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
                auto path = paths.last();
                path->textRect.moveTo(path->textRect.topLeft().x() + xSpan, path->textRect.topLeft().y() + ySpan);
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
                auto path = paths.last();
                if (path->isText && path->textRect.contains(curPoint.toPoint()))
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
            if (path->needDelete)
            {
                paths.removeLast();
                return true;
            }
            path->resetPoint5();
            setDraggerPosition(path->elementAt(0).x, path->elementAt(0).y, path->elementAt(2).x, path->elementAt(2).y);
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
            layerBgPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
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
            path->resetPoint5();
            setDraggerPosition(path->elementAt(0).x, path->elementAt(0).y, path->elementAt(2).x, path->elementAt(2).y);
            state = preState;
            return true;
        }
    }
    return false;
}
