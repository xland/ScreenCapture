
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Canvas.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void initToolMain();
    void initToolRect();
    void showToolMain(int x, int y);
    void showToolSub(int x, int y);

    void hideTool();

private slots:

private:
    void btnMainToolSelected();
    Ui::MainWindow* ui;
    Canvas* canvas;
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
};

#endif // MAINWINDOW_H
