
#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <Windows.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setGeometry(0,0,3560,2440);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void *message, qintptr *result)
{
    MSG *qt_message{reinterpret_cast<MSG *>(message)};
    if(qt_message->message == WM_GETMINMAXINFO)
    {
        MINMAXINFO* mminfo = (PMINMAXINFO)qt_message->lParam;
        mminfo->ptMaxPosition.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        mminfo->ptMaxPosition.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        mminfo->ptMaxSize.x = GetSystemMetrics(SM_CXVIRTUALSCREEN)+1000;
        mminfo->ptMaxSize.y = GetSystemMetrics(SM_CYVIRTUALSCREEN)+800;
        return false;
    }

    return false;
}


