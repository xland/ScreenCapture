
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenPrinter.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ScreenPrinter::Init(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


