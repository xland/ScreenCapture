#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "qdebug.h"


void MainWindow::removeUndoPath()
{
    int index = -1;
    for (size_t i = 0; i < paths.count(); i++)
    {
        if (!paths[i]->needPaint) {
            if (index == -1) {
                index = i;
            }
            qDebug() << "remove00000000000" << index;
            delete paths[i];
        }
    }
    qDebug() << "remove11111111111111111111111" << index;
    if (index != -1) {
        paths.remove(index, paths.count() - index);
        paths.squeeze();
        qDebug() << "remove";
        ui->btnRedo->setStyleSheet("color:#999;");
    }
}

void MainWindow::translateTextToPath()
{
    auto text = this->textInputBox->toPlainText();
    if (text.isEmpty()) {
        this->textInputBox->clear();
        this->textInputBox->hide();
        return;
    }
    auto path = createPath();
    path->isText = true;
    path->text = text;
    path->textRect = this->textInputBox->geometry();
    path->textRect.moveTo(path->textRect.x() + 5, path->textRect.y() + 5);
    path->color = colorSelector->currentColor();
    path->textFont = this->textInputBox->font();
    paintPath(path, layerDrawingPainter);
    isDrawing = true;
    this->textInputBox->clear();
    this->textInputBox->hide();
}

void MainWindow::resizePath(const QPointF& point)
{
    auto& path = paths.last();
    if (draggerIndex == 0)
    {
        path->setElementPositionAt(0, point.x(), point.y());
        path->setElementPositionAt(1, path->elementAt(1).x, point.y());
        path->setElementPositionAt(3, point.x(), path->elementAt(3).y);
        path->setElementPositionAt(4, point.x(), point.y());
    }
    else if (draggerIndex == 1)
    {
        path->setElementPositionAt(0, path->elementAt(0).x, point.y());
        path->setElementPositionAt(1, path->elementAt(1).x, point.y());
        path->setElementPositionAt(4, path->elementAt(4).x, point.y());
    }
    else if (draggerIndex == 2)
    {
        path->setElementPositionAt(0, path->elementAt(0).x, point.y());
        path->setElementPositionAt(1, point.x(), point.y());
        path->setElementPositionAt(2, point.x(), path->elementAt(2).y);
        path->setElementPositionAt(4, path->elementAt(4).x, point.y());
    }
    else if (draggerIndex == 3)
    {
        path->setElementPositionAt(1, point.x(), path->elementAt(1).y);
        path->setElementPositionAt(2, point.x(), path->elementAt(2).y);
    }
    else if (draggerIndex == 4)
    {
        path->setElementPositionAt(1, point.x(), path->elementAt(1).y);
        path->setElementPositionAt(2, point.x(), point.y());
        path->setElementPositionAt(3, path->elementAt(3).x, point.y());
    }
    else if (draggerIndex == 5)
    {
        path->setElementPositionAt(2, path->elementAt(2).x, point.y());
        path->setElementPositionAt(3, path->elementAt(3).x, point.y());
    }
    else if (draggerIndex == 6)
    {
        path->setElementPositionAt(2, path->elementAt(2).x, point.y());
        path->setElementPositionAt(3, point.x(), point.y());
        path->setElementPositionAt(4, point.x(), path->elementAt(4).y);
        path->setElementPositionAt(0, point.x(), path->elementAt(0).y);
    }
    else if (draggerIndex == 7)
    {
        path->setElementPositionAt(3, point.x(), path->elementAt(3).y);
        path->setElementPositionAt(4, point.x(), path->elementAt(4).y);
        path->setElementPositionAt(0, point.x(), path->elementAt(0).y);
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
            path->setElementPositionAt(0, path->elementAt(0).x + xSpan, path->elementAt(0).y + ySpan);
            path->setElementPositionAt(1, path->elementAt(1).x + xSpan, path->elementAt(1).y + ySpan);
            path->setElementPositionAt(2, path->elementAt(2).x + xSpan, path->elementAt(2).y + ySpan);
            path->setElementPositionAt(3, path->elementAt(3).x + xSpan, path->elementAt(3).y + ySpan);
            path->setElementPositionAt(4, path->elementAt(4).x + xSpan, path->elementAt(4).y + ySpan);
            mousePressPoint = point;
        }
    }
    setDraggerPosition(path->elementAt(0).x, path->elementAt(0).y, path->elementAt(2).x, path->elementAt(2).y);
}