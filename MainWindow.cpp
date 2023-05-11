
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include "ScreenShoter.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //| Qt::WindowStaysOnTopHint

    initMask();
    canvasImg = new QImage(ScreenShoter::Get()->screenRects[0].size(), QImage::Format_ARGB32_Premultiplied);
    canvasImg->fill(0);
    painter = new QPainter(canvasImg);
    //todo
    this->showMaximized();
    initToolMain();
    initToolRect();
}

MainWindow::~MainWindow()
{
    delete painter;
    delete canvasImg;
    delete ui;
}

void MainWindow::initMask()
{
    auto screenShoter = ScreenShoter::Get();
    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);

    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter->setPen(Qt::NoPen);
    painter->fillRect(QRect(100, 100, 100, 100), QBrush(Qt::black));

    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->fillRect(QRect(150, 150, 100, 100), QBrush(Qt::black));

    QPainter p(this);
    p.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    p.drawImage(0, 0, *canvasImg);
    p.end();
}

void MainWindow::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->pos();
        isMouseDown = true;
        if (state == State::start)
        {
            auto mainWin = (MainWindow*)(qApp->activeWindow());
            mainWin->hideTool();
        }
        else if (state == State::rect)
        {
            QPainterPath path;
            path.moveTo(mousePressPoint);
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint);
            paths.append(path);
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (isMouseDown)
    {
        auto curPoint = mouseEvent->pos();
        if (state == State::start)
        {
            maskPath.setElementPositionAt(5, mousePressPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(6, curPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(7, curPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(8, mousePressPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(9, mousePressPoint.x(), mousePressPoint.y());
            repaint();
        }
        else if (state == State::rect)
        {
            auto& path = paths.last();
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            repaint();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        isMouseDown = false;
        auto mainWin = (MainWindow*)(qApp->activeWindow());
        //todo 这个位置要动态的，工具条应该出现在正确的位置上
        mainWin->showToolMain(maskPath.elementAt(7).x, maskPath.elementAt(7).y);
    }
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
//        qDebug() << name << btn->isChecked();
        if (btn->isChecked())
        {
            if (name == "btnRectEllipse")
            {
                ui->toolRect->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
                ui->toolRect->show();
                state = State::rect;
            }
            else if (name == "btnArrow")
            {

            }
            break;
        }
    }
}
void MainWindow::hideTool()
{
    ui->toolMain->hide();
}
void MainWindow::showToolMain(int x, int y)
{
    ui->toolMain->move(x - ui->toolMain->width(), y + 6);
    ui->toolMain->show();
}
void MainWindow::showToolSub(int x, int y)
{

}

