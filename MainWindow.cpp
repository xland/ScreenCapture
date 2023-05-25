
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
#include <QFont>
#include <QFontMetrics>



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    colorSelector = new ColorSelector(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //todo | Qt::WindowStaysOnTopHint
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    qApp->installEventFilter(this);
    initMask();
    initDragger();
    initLayer();
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

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        qApp->exit(0);
    }
}

void MainWindow::resizeInputToContent()
{
    ui->textInput->document()->adjustSize();
    auto size = ui->textInput->document()->size().toSize();
    ui->textInput->resize(size.width() + 6, size.height() + 6);

//    auto font = this->ui->textInput->font();
//    font.setPointSize(this->dotText->size + 20);
//    this->ui->textInput->setFont(font);
//    QFontMetrics metrics(font);
//    QString str = this->ui->textInput->toPlainText();
//    if (str.isEmpty())
//    {
//        str = "A";
//    }
//    auto size = metrics.boundingRect(str).size();
//    this->ui->textInput->resize(size.width() + 18, size.height() + 6);
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
    dotPen = new ButtonDot(this);
    dotPen->setCheckable(false);
    auto box = qobject_cast<QHBoxLayout*>(ui->toolPen->layout());
    box->insertWidget(0, dotPen);
    ui->toolPen->hide();
    ui->toolPen->setStyleSheet(style.arg("toolPen"));
}

void MainWindow::initToolMosaic()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->toolMosaic->layout());
    dotMosaic = new ButtonDot(this);
    dotMosaic->setCheckable(false);
    dotMosaic->setFontSize(20);
    box->insertWidget(0, dotMosaic);
    ui->toolMosaic->hide();
    ui->toolMosaic->setStyleSheet(style.arg("toolMosaic"));
}

void MainWindow::initToolEraser()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->toolEraser->layout());
    dotEraser = new ButtonDot(this);
    dotEraser->setCheckable(false);
    dotEraser->setFontSize(20);
    box->insertWidget(0, dotEraser);
    ui->toolEraser->hide();
    ui->toolEraser->setStyleSheet(style.arg("toolEraser"));
}

void MainWindow::initToolRect()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->btnRectEllipseFill->parentWidget()->layout());
    dotRectEllipse = new ButtonDot(this);
    box->insertWidget(0, dotRectEllipse);

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
//    QString style = QString("background:transparent;border:1px solid #000;color:rgb(%1,%2,%3);");
//    ui->textInput->setStyleSheet(style.arg("255", "0", "0"));
//    ui->textInput->setStyleSheet(style.arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue())));

    auto box = qobject_cast<QHBoxLayout*>(ui->toolText->layout());
    dotText = new ButtonDot(this);
    dotText->setFontSize(16);
    dotText->setCheckable(false);
    box->insertWidget(0, dotText);

    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(this->dotText->size + 20);
    this->ui->textInput->setFont(font);

    connect(dotText,  &ButtonDot::sizeChanged, this, [this]()
    {
        auto font = this->ui->textInput->font();
        font.setPointSize(this->dotText->size + 20);
        this->ui->textInput->setFont(font);
        this->resizeInputToContent();
    });

    connect(colorSelector,  &ColorSelector::colorChanged, this, [this]()
    {
        QString style = QString("background:transparent;border:1px solid #000;color:rgb(%1,%2,%3);");
        auto color = colorSelector->currentColor();
        this->ui->textInput->setStyleSheet(style.arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue())));
    });

    connect(ui->fontSelectBox, &QComboBox::currentTextChanged, this, [this]()
    {
        if (this->ui->fontSelectBox->currentText() == "微软雅黑")
        {
            auto font = this->ui->textInput->font();
            font.setFamily("Microsoft YaHei");
            this->ui->textInput->setFont(font);
            this->resizeInputToContent();
        }
        else
        {
            auto font = this->ui->textInput->font();
            font.setFamily("SimSun");
            this->ui->textInput->setFont(font);
            this->resizeInputToContent();
        }
    });

    ui->btnTextBold->setFont(Icon::font);
    ui->btnTextBold->setText(Icon::icons[Icon::Name::bold]);
    connect(ui->btnTextBold,  &QPushButton::clicked, this, [this]()
    {
        QFont font = this->ui->textInput->font();
        font.setBold(this->ui->btnTextBold->isChecked());
        this->ui->textInput->setFont(font);
    });

    ui->btnTextItalic->setFont(Icon::font);
    ui->btnTextItalic->setText(Icon::icons[Icon::Name::italic]);
    connect(ui->btnTextItalic,  &QPushButton::clicked, this, [this]()
    {
        QFont font = this->ui->textInput->font();
        font.setItalic(ui->btnTextItalic->isChecked());
        this->ui->textInput->setFont(font);
    });

    ui->toolText->hide();
    ui->toolText->setStyleSheet(style.arg("toolText"));

    ui->textInput->hide();
    connect(ui->textInput, &QTextEdit::textChanged, this, &MainWindow::resizeInputToContent);
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
            tool->hide();
            colorSelector->hide();
            state = name.remove("tool");
            tool->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
            setCursor(Qt::CrossCursor);
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

