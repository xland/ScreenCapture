#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"

//todo 移动位置和大小后，没法还原

void MainWindow::undo()
{
    bool flag = false;
    int var = paths.count() - 1;
    for (; var >= 0; --var)
    {
        if (paths[var].needPaint)
        {
            paths[var].needPaint = false;
            flag = true;
            break;
        }
    }
    if (!flag)
    {
        return;
    }
    isDrawing = false;
    showDraggerCount = 0;
    layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            break;
        }
        if (paths[var].isEraser)
        {
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
            paintPath(paths[var], layerDrawingPainter);
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
        }
        else
        {
            paintPath(paths[var], layerBgPainter);
        }

    }
    repaint();
    ui->btnRedo->setStyleSheet("");
    if (var == 0)
    {
        ui->btnUndo->setStyleSheet("color:#999;");
    }
    else
    {
        ui->btnUndo->setStyleSheet("");
    }

}
void MainWindow::redo()
{
    bool flag = false;
    int var = 0;
    for (; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            paths[var].needPaint = true;
            flag = true;
            break;
        }
    }
    if (!flag)
    {
        return;
    }
    showDraggerCount = 0;
    isDrawing = false;
    layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            break;
        }
        if (paths[var].isEraser)
        {
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            layerBgPainter->drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
            paintPath(paths[var], layerDrawingPainter);
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
        }
        else
        {
            paintPath(paths[var], layerBgPainter);
        }
    }
    repaint();
    ui->btnUndo->setStyleSheet("");
    if (var == paths.count() - 1)
    {
        ui->btnRedo->setStyleSheet("color:#999;");
    }
    else
    {
        ui->btnRedo->setStyleSheet("");
    }

}
