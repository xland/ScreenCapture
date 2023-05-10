
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Icon.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void initTool();
    void showToolMain(int x, int y);
    void showToolSub(int x, int y);
public:
    Icon::Name selectedIcon = Icon::Name::none;

private slots:

private:
    Ui::MainWindow* ui;
    QString selectedBtnStyle = "background:#1677ff;color:#fff;";
};

#endif // MAINWINDOW_H
