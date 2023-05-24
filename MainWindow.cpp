
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"
#include <QPen>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QPainterPath>
#include <QCoreApplication>
#include <QLineEdit>
#include <QLayout>



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    colorSelector = new ColorSelector(this);
    buttonDot = new ButtonBot(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //todo | Qt::WindowStaysOnTopHint
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    qApp->installEventFilter(this);
    initMask();
    initDragger();
    initCanvasImg();
    initToolMain();
    initToolRect();
    initToolArrow();
    initToolMosaic();
    initToolPen();
    initToolEraser();
    initToolText();
    this->showMaximized(); //todo
}

MainWindow::~MainWindow()
{

    delete layerDrawingPainter;
    delete layerBgPainter;
    delete layerDrawingImg;
    delete layerBgImg;
    delete ui;
}

void MainWindow::resizeInputToContent()
{
    auto font = ui->lineEdit->font();
    QFontMetrics fm(font);
    auto size = fm.size(Qt::TextSingleLine, ui->lineEdit->text());
    auto w = size.width() + 18;
    auto h = size.height() + 12;
    if (w < 12) w = 12;
    ui->lineEdit->setFixedSize(w, h);
}

void MainWindow::initToolMain()
{
    ui->btnRectEllipse->setFont(Icon::font);
    ui->btnRectEllipse->setText(Icon::icons[Icon::Name::rectEllipse]);
    QObject::connect(ui->btnRectEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrow->setFont(Icon::font);
    ui->btnArrow->setText(Icon::icons[Icon::Name::arrow]);
    connect(ui->btnArrow,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnPen->setFont(Icon::font);
    ui->btnPen->setText(Icon::icons[Icon::Name::line]);
    QObject::connect(ui->btnPen,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnMosaic->setFont(Icon::font);
    ui->btnMosaic->setText(Icon::icons[Icon::Name::mosaic]);
    QObject::connect(ui->btnMosaic,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnUndo->setFont(Icon::font);
    ui->btnUndo->setText(Icon::icons[Icon::Name::undo]);
    QObject::connect(ui->btnUndo,  &QPushButton::clicked, this, &MainWindow::undo);

    ui->btnRedo->setFont(Icon::font);
    ui->btnRedo->setText(Icon::icons[Icon::Name::redo]);
    QObject::connect(ui->btnRedo,  &QPushButton::clicked, this, &MainWindow::redo);

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

void MainWindow::initToolPen()
{
    ui->btnPenDot->setFont(Icon::font);
    ui->btnPenDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnPenDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolPen->hide();
    ui->toolPen->setStyleSheet(style.arg("toolPen"));
}

void MainWindow::initToolMosaic()
{
    ui->btnMosaicDot->setFont(Icon::font);
    ui->btnMosaicDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnMosaicDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolMosaic->hide();
    ui->toolMosaic->setStyleSheet(style.arg("toolMosaic"));
}

void MainWindow::initToolEraser()
{
    ui->btnEraserDot->setFont(Icon::font);
    ui->btnEraserDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnEraserDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEraserRect->setFont(Icon::font);
    ui->btnEraserRect->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnEraserRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolEraser->hide();
    ui->toolEraser->setStyleSheet(style.arg("toolEraser"));
}

void MainWindow::initToolRect()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->btnRectEllipseFill->parentWidget()->layout());
    box->insertWidget(0, buttonDot);

    ui->btnRectEllipseFill->setFont(Icon::font);
    ui->btnRectEllipseFill->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnRectEllipseFill,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnRect->setFont(Icon::font);
    ui->btnRect->setText(Icon::icons[Icon::Name::rect]);
    QObject::connect(ui->btnRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEllipse->setFont(Icon::font);
    ui->btnEllipse->setText(Icon::icons[Icon::Name::ellipse]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolRectEllipse->hide();
    ui->toolRectEllipse->setStyleSheet(style.arg("toolRectEllipse"));
}

void MainWindow::initToolArrow()
{
    ui->btnArrowFill->setFont(Icon::font);
    ui->btnArrowFill->setText(Icon::icons[Icon::Name::arrowFill]);
    QObject::connect(ui->btnArrowFill,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrowEmpty->setFont(Icon::font);
    ui->btnArrowEmpty->setText(Icon::icons[Icon::Name::arrowEmpty]);
    QObject::connect(ui->btnArrowEmpty,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolArrow->hide();
    ui->toolArrow->setStyleSheet(style.arg("toolArrow"));
}

void MainWindow::initToolText()
{
    ui->btnTextDot->setFont(Icon::font);
    ui->btnTextDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnTextDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnTextBold->setFont(Icon::font);
    ui->btnTextBold->setText(Icon::icons[Icon::Name::bold]);
    QObject::connect(ui->btnTextBold,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnTextItalic->setFont(Icon::font);
    ui->btnTextItalic->setText(Icon::icons[Icon::Name::italic]);
    QObject::connect(ui->btnTextItalic,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolText->hide();
    ui->toolText->setStyleSheet(style.arg("toolText"));

    ui->lineEdit->hide();
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::resizeInputToContent);
}

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
            if (state == "Mosaic")
            {
                layerBgPainter->drawImage(0, 0, *layerDrawingImg);
            }
            state = name.remove("tool");
            tool->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
            setCursor(Qt::CrossCursor);
            tool->show();
            if (state == "Mosaic")
            {
                initMosaicImg();
            }
            else
            {
                if (state != "Eraser")
                {
//                    if (!tool->property("hasSetColor").isValid())
//                    {
//                        tool->setProperty("hasSetColor", true);
//                        tool->resize(tool->width() + colorSelector->width(), tool->height());
//                    }
//                    tool->layout()->addWidget(colorSelector);
                    colorSelector->move(tool->geometry().topRight());
                    colorSelector->show();
                }
            }
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

