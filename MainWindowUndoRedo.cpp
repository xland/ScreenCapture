#include "MainWindow.h"
#include "ui_MainWindow.h"

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
    showDraggerCount = 0;
    canvasImg2->fill(0);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            break;
        }
        paintPath(paths[var], painter2);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
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
    canvasImg2->fill(0);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (!paths[var].needPaint)
        {
            break;
        }
        paintPath(paths[var], painter2);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
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
