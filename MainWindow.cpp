
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"
#include <QPen>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QPainterPath>
#include <QCoreApplication>
#include "ScreenShoter.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //todo | Qt::WindowStaysOnTopHint
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    this->installEventFilter(this);
    initMask();
    initCanvasImg();
    initToolMain();
    initToolRect();
    initToolEraser();
    this->showMaximized(); //todo
}

MainWindow::~MainWindow()
{
    delete painter1;
    delete painter2;
    delete canvasImg1;
    delete canvasImg2;
    delete ui;
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    if (event->type() == QEvent::MouseMove)
    {
        mouseMove(e);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        mousePress(e);
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        return mouseRelease(e);
    }
    return false;
}
void MainWindow::initCanvasImg()
{
    canvasImg1 = new QImage(ScreenShoter::Get()->screenRects[0].size(), QImage::Format_ARGB32_Premultiplied);
    canvasImg1->fill(0);
    canvasImg2 = new QImage(ScreenShoter::Get()->screenRects[0].size(), QImage::Format_ARGB32_Premultiplied);
    canvasImg2->fill(0);
    painter1 = new QPainter(canvasImg1);
    painter1->setRenderHint(QPainter::Antialiasing);
    painter2 = new QPainter(canvasImg2);
    painter2->setRenderHint(QPainter::Antialiasing);
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
    QPainter p(this);
    p.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    p.drawImage(0, 0, *canvasImg1);
    p.setPen(QPen(QBrush(QColor(22, 119, 255)), maskBorderWidth));
    p.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.drawPath(maskPath);
    p.end();
}

bool MainWindow::mousePress(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return true;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->pos();
        isMouseDown = true;
        if (state == "Start")
        {
            ui->toolMain->hide();
        }
        else if (state == "RectEllipse")
        {
            QPainterPath path;
            path.moveTo(mousePressPoint);
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y());
            path.lineTo(mousePressPoint.x() + 1, mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint.x(), mousePressPoint.y() + 1);
            path.lineTo(mousePressPoint);
            paths.append(path);

        }
        else if (state == "Eraser")
        {
            QPainterPath path;
            path.moveTo(mousePressPoint);
            paths.append(path);
            painter1->setCompositionMode(QPainter::CompositionMode_DestinationOver);
            canvasImg1->fill(0);
            painter1->drawImage(0, 0, *canvasImg2);
        }
        return true;
    }
    return false;
}

bool MainWindow::mouseMove(QMouseEvent* mouseEvent)
{
    auto curPoint = mouseEvent->pos();
    if (isMouseDown)
    {
        if (state == "Start")
        {
            maskPath.setElementPositionAt(5, mousePressPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(6, curPoint.x(), mousePressPoint.y());
            maskPath.setElementPositionAt(7, curPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(8, mousePressPoint.x(), curPoint.y());
            maskPath.setElementPositionAt(9, mousePressPoint.x(), mousePressPoint.y());
            repaint();
            return true;
        }
        else if (state == "RectEllipse")
        {
            auto& path = paths.last();
            path.setElementPositionAt(1, curPoint.x(), mousePressPoint.y());
            path.setElementPositionAt(2, curPoint.x(), curPoint.y());
            path.setElementPositionAt(3, mousePressPoint.x(), curPoint.y());
            memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
            painter1->setPen(QPen(QBrush(Qt::red), 2));
            painter1->setBrush(Qt::NoBrush);
            painter1->drawPath(path);
            repaint();
            return true;
        }
        else if (state == "Eraser")
        {
            auto& path = paths.last();
            path.lineTo(curPoint.x(), curPoint.y());
            painter1->setRenderHint(QPainter::Antialiasing);
            painter1->setCompositionMode(QPainter::CompositionMode_Clear);
            painter1->setPen(QPen(QBrush(Qt::red), 12));
            painter1->setBrush(Qt::NoBrush);
            painter1->drawPath(path);
            repaint();
            return true;
        }
    }
    else
    {
        if (state == "areaReady")
        {
//            if (ui->toolMain->geometry().contains(curPoint))
//            {
//                qDebug() << ui->toolMain->geometry() << curPoint;
//                return false;
//            }
            if (curPoint.x() < maskPath.elementAt(5).x + maskBorderWidth && curPoint.y() < maskPath.elementAt(5).y + maskBorderWidth)
            {
                setCursor(Qt::SizeFDiagCursor);
            }
            else if (curPoint.x() > maskPath.elementAt(6).x - maskBorderWidth && curPoint.y() < maskPath.elementAt(6).y + maskBorderWidth)
            {
                setCursor(Qt::SizeBDiagCursor);
            }
            else if (curPoint.x() > maskPath.elementAt(7).x - maskBorderWidth && curPoint.y() > maskPath.elementAt(7).y - maskBorderWidth)
            {
                setCursor(Qt::SizeFDiagCursor);
            }
            else if (curPoint.x() < maskPath.elementAt(8).x + maskBorderWidth && curPoint.y() > maskPath.elementAt(8).y - maskBorderWidth)
            {
                setCursor(Qt::SizeBDiagCursor);
            }
            else if (curPoint.x() > maskPath.elementAt(5).x + maskBorderWidth && curPoint.y() > maskPath.elementAt(5).y + maskBorderWidth &&
                curPoint.x() < maskPath.elementAt(7).x - maskBorderWidth && curPoint.y() < maskPath.elementAt(7).y - maskBorderWidth
            )
            {
                setCursor(Qt::SizeAllCursor);
            }
            else if (curPoint.x() < maskPath.elementAt(5).x + maskBorderWidth)
            {
                setCursor(Qt::SizeHorCursor);
            }
            else if (curPoint.x() > maskPath.elementAt(6).x - maskBorderWidth)
            {
                setCursor(Qt::SizeHorCursor);
            }
            else if (curPoint.y() < maskPath.elementAt(5).y + maskBorderWidth)
            {
                setCursor(Qt::SizeVerCursor);
            }
            else if (curPoint.y() > maskPath.elementAt(7).y - maskBorderWidth)
            {
                setCursor(Qt::SizeVerCursor);
            }
            return false;
        }
        else if (state == "RectEllipse")
        {
            if (paths.count() > 0)
            {
                auto& path = paths.last();
                if (path.contains(curPoint))
                {
                    setCursor(Qt::SizeAllCursor);
                }
                else
                {
                    setCursor(Qt::CrossCursor);
                }
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::mouseRelease(QMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        isMouseDown = false;
        auto mainWin = (MainWindow*)(qApp->activeWindow());
        if (state == "Start")
        {
            qreal x2 = -999.0, x1 = 999999999.0;
            qreal y2 = -999.0, y1 = 999999999.0;
            for (int var = 5; var < 9; ++var)
            {
                auto ele = maskPath.elementAt(var);
                if (ele.x < x1)
                {
                    x1 = ele.x;
                }
                if (ele.x > x2)
                {
                    x2 = ele.x;
                }
                if (ele.y < y1)
                {
                    y1 = ele.y;
                }
                if (ele.y > y2)
                {
                    y2 = ele.y;
                }
            }
            maskPath.setElementPositionAt(5, x1, y1);
            maskPath.setElementPositionAt(6, x2, y1);
            maskPath.setElementPositionAt(7, x2, y2);
            maskPath.setElementPositionAt(8, x1, y2);
            maskPath.setElementPositionAt(9, x1, y1);
            //todo 这个位置要动态的，工具条应该出现在正确的位置上
            mainWin->showToolMain(x2, y2);
            state = "areaReady";
            return true;
        }
        else if (state == "RectEllipse")
        {
            auto& path = paths.last();
            qreal x2 = -999.0, x1 = 999999999.0;
            qreal y2 = -999.0, y1 = 999999999.0;
            for (int var = 0; var < 5; ++var)
            {
                auto ele = path.elementAt(var);
                if (ele.x < x1)
                {
                    x1 = ele.x;
                }
                if (ele.x > x2)
                {
                    x2 = ele.x;
                }
                if (ele.y < y1)
                {
                    y1 = ele.y;
                }
                if (ele.y > y2)
                {
                    y2 = ele.y;
                }
            }
            path.setElementPositionAt(0, x1, y1);
            path.setElementPositionAt(1, x2, y1);
            path.setElementPositionAt(2, x2, y2);
            path.setElementPositionAt(3, x1, y2);
            path.setElementPositionAt(4, x1, y1);
            painter2->setPen(QPen(QBrush(Qt::red), 2));
            painter2->setBrush(Qt::NoBrush);
            painter2->drawPath(path);
            setCursor(Qt::CrossCursor);
            return true;
        }
    }
    return false;
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

    ui->toolRectEllipse->hide();
    ui->toolRectEllipse->setStyleSheet(style.arg("toolRectEllipse"));
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
            state = name.remove("tool");
            setCursor(Qt::CrossCursor);
            tool->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
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
    for (auto item : ui->toolMain->children())
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

void MainWindow::showToolMain(int x, int y)
{
    //todo 计算合适的位置
    ui->toolMain->move(x - ui->toolMain->width(), y + 6);
    ui->toolMain->show();
    ui->toolMain->setFocus(Qt::OtherFocusReason);
}

