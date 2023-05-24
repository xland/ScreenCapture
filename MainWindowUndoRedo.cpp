#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"

//todo 移动位置和大小后，没法还原
void MainWindow::jobLoop()
{
    isDrawing = false;
    showDraggerCount = 0;
    ScreenShoter::Get()->desktopImages[0].save("desktopImages.png");
    memcpy(layerBgImg->bits(), ScreenShoter::Get()->desktopImages[0].bits(), layerBgImg->sizeInBytes());
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            break;
        }
        if (paths[var].isEraser)
        {
            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
            memcpy(layerBgImg->bits(), ScreenShoter::Get()->desktopImages[0].bits(), layerBgImg->sizeInBytes());
            paintPath(paths[var], layerDrawingPainter);
//            while (paths[var + 1].isEraser && paths[var + 1].needPaint)
//            {
//                var += 1;
//                paintPath(paths[var], layerDrawingPainter);
//            }
            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
        }
        else if (paths[var].isMosaic)
        {
            initMosaic();
            paintPath(paths[var], layerBgPainter);
//            while (paths[var + 1].isMosaic && paths[var + 1].needPaint)
//            {
//                var += 1;
//                paintPath(paths[var], layerBgPainter);
//            }
            memcpy(layerDrawingImg->bits(), layerMosaicImg->bits(), layerDrawingImg->sizeInBytes());
            layerDrawingPainter->drawImage(0, 0, *layerBgImg);
            memcpy(layerBgImg->bits(), layerDrawingImg->bits(), layerBgImg->sizeInBytes());
            layerDrawingImg->fill(0);
        }
        else
        {
            paintPath(paths[var], layerBgPainter);
        }
    }
    repaint();
}
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
    jobLoop();
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
    jobLoop();
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
