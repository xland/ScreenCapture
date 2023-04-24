
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QGraphicsDropShadowEffect>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#666"));
    shadow->setBlurRadius(8);
    ui->centralwidget->setGraphicsEffect(shadow);

    //设置登录操作区域阴影
    QGraphicsDropShadowEffect* shadowFrameLogin = new QGraphicsDropShadowEffect(this);
    shadowFrameLogin->setOffset(0, 0);
    shadowFrameLogin->setColor(QColor("#DDD"));
    shadowFrameLogin->setBlurRadius(26);
    ui->widgetLogin->setGraphicsEffect(shadowFrameLogin);
}

MainWindow::~MainWindow()
{
    delete ui;
}


