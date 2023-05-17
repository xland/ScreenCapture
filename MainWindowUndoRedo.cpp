#include "MainWindow.h"
#include "ui_MainWindow.h"


void MainWindow::undo()
{
    if (paths.count() < 1)
    {
        return;
    }
    if (undoIndex == 0)
    {
        return;
    }
    else if (undoIndex == -1)
    {
        undoIndex = paths.count() - 1;
    }
    else
    {
        undoIndex -= 1;
    }
    showDraggerCount = 0;
    canvasImg2->fill(0);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (var >= undoIndex)
        {
            break;
        }
        paintPath(paths[var]);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    repaint();
    ui->btnRedo->setStyleSheet("");
    if (undoIndex == 0)
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
    if (undoIndex == -1)
    {
        return;
    }
    showDraggerCount = 0;
    undoIndex += 1;
    canvasImg2->fill(0);
    ui->btnUndo->setStyleSheet("");
    for (int var = 0; var < paths.count(); ++var)
    {
        if (var >= undoIndex)
        {
            break;
        }
        paintPath(paths[var]);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    repaint();
    if (undoIndex >= paths.count())
    {
        ui->btnRedo->setStyleSheet("color:#999;");
        undoIndex = -1;
    }
}
