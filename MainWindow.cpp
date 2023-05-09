
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenShoter.h"
#include <QPixmap>
#include <QPainter>
#include <QGraphicsPixmapItem>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //| Qt::WindowStaysOnTopHint
    this->showMaximized();//todo
    scene = new QGraphicsScene(ui->graphicsView);
    scene->setSceneRect(0.0, 0.0, this->width(), this->height());
    ui->graphicsView->setScene(scene);
    auto imgItem = scene->addPixmap(*(ScreenShoter::Get()->desktopImage));

//    auto imgCur = img->scaled(QSize(446, 440) * scalFactor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    imgCur.setDevicePixelRatio(scalFactor);
    imgItem->setPos(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


