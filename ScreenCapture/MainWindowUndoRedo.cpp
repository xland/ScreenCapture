#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"

//todo 移动位置和大小后，没法还原
//todo 上一步之后，画了新东西，要清除内存
//todo 按右键的时候需要执行一次 endOneDraw
void MainWindow::jobLoop()
{
    endOneDraw();
    memcpy(layerBgImg->bits(), ScreenShoter::Get()->desktopImages[0].bits(), layerBgImg->sizeInBytes());
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var]->needPaint)
        {
            break;
        }
        if (paths[var]->isEraser)
        {
//            memcpy(layerDrawingImg->bits(), layerBgImg->bits(), layerDrawingImg->sizeInBytes());
//            memcpy(layerBgImg->bits(), ScreenShoter::Get()->desktopImages[0].bits(), layerBgImg->sizeInBytes());
//            paintPath(paths[var], layerDrawingPainter);
//            layerBgPainter->drawImage(0, 0, *layerDrawingImg);
            layerBgPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            layerBgPainter->drawImage(paths[var]->patchPosition, paths[var]->patchImg);
        }
        else if (paths[var]->isMosaic)
        {
//            initMosaic();
//            paintPath(paths[var], layerBgPainter);
//            memcpy(layerDrawingImg->bits(), layerMosaicImg->bits(), layerDrawingImg->sizeInBytes());
//            layerDrawingPainter->drawImage(0, 0, *layerBgImg);
//            memcpy(layerBgImg->bits(), layerDrawingImg->bits(), layerBgImg->sizeInBytes());
//            layerDrawingImg->fill(0);
            layerBgPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            layerBgPainter->drawImage(paths[var]->patchPosition, paths[var]->patchImg);
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
        if (paths[var]->needPaint)
        {
            paths[var]->needPaint = false;
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
        if (!paths[var]->needPaint)
        {
            paths[var]->needPaint = true;
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
