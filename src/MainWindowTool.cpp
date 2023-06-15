#include "MainWindow.h"
#include "ui_MainWindow.h"



void MainWindow::switchTool(const QString& toolName)
{
    for (auto item : this->centralWidget()->children())
    {
        auto name = item->objectName();
        auto tool = qobject_cast<QWidget*>(item);
        if (!tool || name == "toolMain")
        {
            continue;
        }
        if (name == toolName)
        {
            tool->hide();
            colorSelector->hide();
            state = name.remove("tool");
            tool->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
            if(state == "Text"){
                setCursor(Qt::IBeamCursor);
            }else{
                setCursor(Qt::CrossCursor);
            }            
            if (state == "Mosaic")
            {
                initMosaic();
            }
            else
            {
                if (state != "Eraser")
                {
                    
                    colorSelector->move(tool->geometry().topRight());
                    colorSelector->show();
                }
            }
            tool->show();
        }
        else
        {
            tool->hide();
        }
    }
}


void MainWindow::btnMainToolSelected()
{
    for (auto item : ui->mainToolMenuContainer->children())
    {
        auto name = item->objectName();
        auto btn = qobject_cast<QPushButton*>(item);
        if (!btn)
        {
            continue;
        }
        if (btn->isChecked())
        {
            endOneDraw();
            switchTool(name.replace("btn", "tool"));
            break;
        }
    }
}

void MainWindow::showToolMain()
{
    auto ele0 = maskPath.elementAt(0);
    auto ele1 = maskPath.elementAt(1);
    auto ele2 = maskPath.elementAt(2);
    if (this->height() - ele2.y > 80)
    {
        auto x = ele2.x - ui->toolMain->width();
        if (x > 0)
        {
            ui->toolMain->move(x, ele2.y + 6);
        }
        else
        {
            ui->toolMain->move(ele0.x, ele2.y + 6);
        }
        ui->toolMain->show();
        return;
    }
    if (ele1.y > 80)
    {
        auto x = ele1.x - ui->toolMain->width();
        if (x > 0)
        {
            ui->toolMain->move(x, ele1.y - 6 - 32);
        }
        else
        {
            ui->toolMain->move(ele0.x, ele1.y - 6 - 32);
        }
        ui->toolMain->show();
        return;
    }
    auto x = ele1.x - ui->toolMain->width() - 6;
    if (x > 0)
    {
        ui->toolMain->move(x, ele1.y + 6);
    }
    else
    {
        ui->toolMain->move(ele0.x + 6, ele1.y + 6);
    }
    ui->toolMain->show();
}




