
#include "MainWindow.h"
#include "ui_MainWindow.h"
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
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //todo | Qt::WindowStaysOnTopHint
    this->showMaximized(); //todo
    ui->tipBox->setMouseTracking(false);
    ui->tipBox->hide();
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    colorSelector = new ColorSelector(this);    
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
    //ui->centralwidget->layout()->addWidget(textBox);
    //    moveTipBox();
}

MainWindow::~MainWindow()
{
    delete layerDrawingPainter;
    delete layerBgPainter;
    delete layerMosaicPainter;

    delete layerDrawingImg;
    delete layerBgImg;
    delete layerMosaicImg;

    delete ui;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (state != "Start")
    {
        endOneDraw();
        saveToClipboard();
    }
}

void MainWindow::moveTipBox()
{
    auto position = QCursor::pos();
    ui->tipBox->move(position.x() + 10, position.y() + 10);
    if (this->width() - position.x() < ui->tipBox->width())
    {
        ui->tipBox->move(position.x() - ui->tipBox->width() - 10, ui->tipBox->y());
    }
    if (this->height() - position.y() < ui->tipBox->height())
    {
        ui->tipBox->move(ui->tipBox->x(), position.y() - ui->tipBox->height() - 10);
    }
    ui->tipPositionLabel->setText("位置: X: " + QString::number(position.x()) + "  Y: " + QString::number(position.y()));
    position = position * scaleFactor;
    auto color = layerBgImg->pixelColor(position);
    auto rgbStr = QString("%1,%2,%3").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
    auto hexStr = color.name(QColor::HexRgb).toUpper();
    ui->tipRgbLabel->setText("RGB: " + rgbStr + "  HEX: " + hexStr);
    int x{0}, y{0}, width = ui->tipImageLabel->width() * scaleFactor, height = ui->tipImageLabel->height() * scaleFactor;
    x = position.x() - width / 16;
    y = position.y() - height / 16;
    QRect rect(x, y, width / 8, height / 8);
    QImage img = layerBgImg->copy(rect).scaled(width, height);
    for (int i = 0; i < img.width(); i++)
    {
        img.setPixelColor({ i,0 }, Qt::black);
    }
    for (int i = 0; i < img.height(); i++)
    {
        img.setPixelColor({ 0,i }, Qt::black);
        img.setPixelColor({ img.width()-2,i }, Qt::black);
    }

    
    QPainter p(&img);
    int w = img.width() / scaleFactor;
    int h = img.height() / scaleFactor;
    int size = 3;
    QBrush brush(QColor(128, 128, 255, 128));
    p.fillRect(QRect(0, h / 2 - size, w / 2 - size, size * 2), brush);
    p.fillRect(QRect(w / 2 + size, h / 2 - size, w / 2 - size, size * 2), brush);
    p.fillRect(QRect(w / 2 - size, 0, size * 2, h / 2 - size), brush);
    p.fillRect(QRect(w / 2 - size, h / 2 + size, size * 2, h / 2 - size), brush);
    ui->tipImageLabel->setPixmap(QPixmap::fromImage(img));
    ui->tipImageLabel->setScaledContents(true);
    ui->tipBox->show();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        qApp->exit(0);
    }
    else if (event->modifiers() == Qt::AltModifier)
    {
        if (event->key() == Qt::Key_X)
        {
            auto position = QCursor::pos() * scaleFactor;
            auto color = layerBgImg->pixelColor(position);
            auto rgbStr = QString("rgb(%1,%2,%3)")
                .arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
            QApplication::clipboard()->setText(rgbStr);
        }
        else if (event->key() == Qt::Key_C)
        {
            auto position = QCursor::pos() * scaleFactor;
            auto color = layerBgImg->pixelColor(position);
            auto hexStr = color.name(QColor::HexRgb).toUpper();
            QApplication::clipboard()->setText(hexStr);
        }
    }
}


void MainWindow::saveToFile()
{
    auto filePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    filePath = QDir::cleanPath(filePath + QDir::separator() + "ScreenCapture" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png");
    filePath = QFileDialog::getSaveFileName(this, "保存文件", filePath, "Screen Capture (*.png)");
    if (filePath.isEmpty())
    {
        return;
    }
    endOneDraw();
    QRect rect;
    rect.adjust(maskPath.elementAt(0).x * scaleFactor - maskBorderWidth,
        maskPath.elementAt(0).y * scaleFactor - maskBorderWidth,
        maskPath.elementAt(2).x * scaleFactor - maskBorderWidth,
        maskPath.elementAt(2).y * scaleFactor - maskBorderWidth);
    layerBgImg->copy(rect).save(filePath);
    qApp->exit(0);
}

void MainWindow::saveToClipboard()
{
    endOneDraw();
    QRect rect;
    rect.adjust(maskPath.elementAt(0).x * scaleFactor - maskBorderWidth,
        maskPath.elementAt(0).y * scaleFactor - maskBorderWidth,
        maskPath.elementAt(2).x * scaleFactor - maskBorderWidth,
        maskPath.elementAt(2).y * scaleFactor - maskBorderWidth);
    auto img = layerBgImg->copy(rect);
    QApplication::clipboard()->setImage(img);
    qApp->exit(0);
}

