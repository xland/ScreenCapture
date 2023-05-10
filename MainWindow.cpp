
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QDebug>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //| Qt::WindowStaysOnTopHint

    //todo
    this->showMaximized();

    canvas = new Canvas(ui->graphicsView);
    ui->graphicsView->setScene(canvas);
    initToolMain();
    initToolRect();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initToolMain()
{
    ui->btnRectEllipse->setFont(Icon::font);
    ui->btnRectEllipse->setText(Icon::icons[Icon::Name::rectEllipse]);
    QObject::connect(ui->btnRectEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrow->setFont(Icon::font);
    ui->btnArrow->setText(Icon::icons[Icon::Name::arrow]);
    QObject::connect(ui->btnArrow,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnLine->setFont(Icon::font);
    ui->btnLine->setText(Icon::icons[Icon::Name::line]);
    QObject::connect(ui->btnLine,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnMosaic->setFont(Icon::font);
    ui->btnMosaic->setText(Icon::icons[Icon::Name::mosaic]);
    QObject::connect(ui->btnMosaic,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnUndo->setFont(Icon::font);
    ui->btnUndo->setText(Icon::icons[Icon::Name::undo]);
    QObject::connect(ui->btnUndo,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnRedo->setFont(Icon::font);
    ui->btnRedo->setText(Icon::icons[Icon::Name::redo]);
    QObject::connect(ui->btnRedo,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnOk->setFont(Icon::font);
    ui->btnOk->setText(Icon::icons[Icon::Name::ok]);
    QObject::connect(ui->btnOk,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnSave->setFont(Icon::font);
    ui->btnSave->setText(Icon::icons[Icon::Name::save]);
    QObject::connect(ui->btnSave,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnCopy->setFont(Icon::font);
    ui->btnCopy->setText(Icon::icons[Icon::Name::copy]);
    QObject::connect(ui->btnCopy,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnText->setFont(Icon::font);
    ui->btnText->setText(Icon::icons[Icon::Name::text]);
    QObject::connect(ui->btnText,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEraser->setFont(Icon::font);
    ui->btnEraser->setText(Icon::icons[Icon::Name::eraser]);
    QObject::connect(ui->btnEraser,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnNumber->setFont(Icon::font);
    ui->btnNumber->setText(Icon::icons[Icon::Name::number]);
    QObject::connect(ui->btnNumber,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolMain->hide();
    ui->toolMain->setStyleSheet(style.arg("toolMain"));

}

void MainWindow::initToolRect()
{
    ui->btnDot->setFont(Icon::font);
    ui->btnDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnFill->setFont(Icon::font);
    ui->btnFill->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnRect->setFont(Icon::font);
    ui->btnRect->setText(Icon::icons[Icon::Name::rect]);
    QObject::connect(ui->btnRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEllipse->setFont(Icon::font);
    ui->btnEllipse->setText(Icon::icons[Icon::Name::ellipse]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolRect->hide();
    ui->toolRect->setStyleSheet(style.arg("toolRect"));
}

void MainWindow::btnMainToolSelected()
{
    for (auto item : ui->toolMain->children())
    {
        auto btn = qobject_cast<QPushButton*>(item);
        if (!btn)
        {
            continue;
        }
        auto name = item->objectName();
        qDebug() << name << btn->isChecked();
        if (btn->isChecked())
        {
            if (name == "btnRectEllipse")
            {
                ui->toolRect->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 6);
                ui->toolRect->show();
                canvas->state = Canvas::State::rect;
            }
            else if (name == "btnArrow")
            {

            }
            break;
        }
    }
}

void MainWindow::showToolMain(int x, int y)
{
    ui->toolMain->move(x - ui->toolMain->width(), y + 10);
    ui->toolMain->show();
}
void MainWindow::showToolSub(int x, int y)
{

}

