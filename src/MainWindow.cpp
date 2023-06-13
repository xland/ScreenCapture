
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <Windows.h>
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
#include <QScreen>
#include <QMimeData>
#include <QBuffer>
#include <QTimer>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose,true);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint|Qt::SubWindow);   //| Qt::WindowStaysOnTopHint
    setFocusPolicy(Qt::StrongFocus);
    initWindowRects();
    shotScreen();
    adjustWindowToScreen();
    connect(this,&MainWindow::shotScreenReady,this,&MainWindow::initWindow);
}

MainWindow::~MainWindow()
{
    delete layerDrawingPainter;
    delete layerBgPainter;
    delete layerMosaicPainter;

    delete layerDrawingImg;
    delete layerBgImg;
    delete layerMosaicImg;
    delete desktopImage;

    delete ui;
}

void MainWindow::initWindow()
{
    initLayer();
    setFocus();
    ui->tipBox->setMouseTracking(false);
    ui->tipBox->hide();
    colorSelector = new ColorSelector(this);    
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    qApp->installEventFilter(this);
    initMask();
    initDragger();
    initToolMain();
    initToolRect();
    initToolArrow();
    initToolMosaic();
    initToolPen();
    initToolEraser();
    initToolText();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (state != "Start")
    {
        saveToClipboard();
    }
}

void MainWindow::moveTipBox()
{
    QPoint position = getNativeMousePos();
    ui->tipBox->move(position.x()+10, position.y()+10);
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

void MainWindow::highlightWindow()
{
    POINT pos;
    GetCursorPos(&pos);
    for (size_t i = 0; i < windowRects.count(); i++)
    {
        auto& rect = windowRects[i];
        if(rect.contains(pos.x,pos.y)){
            POINT p1 {rect.topLeft().x(),rect.topLeft().y()};
            ScreenToClient(hwnd, &p1);
            POINT p2 {rect.bottomRight().x(),rect.bottomRight().y()};
            ScreenToClient(hwnd, &p2);
            setMask(QPoint(p1.x,p1.y)/scaleFactor, QPoint{p2.x,p2.y}/scaleFactor);
            break;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        qApp->exit(0);
    }
    else if (event->key() == Qt::Key_Left) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x - 1,pos.y);
    }
    else if (event->key() == Qt::Key_Up) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x,pos.y-1);
    }
    else if (event->key() == Qt::Key_Down) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x,pos.y+1);
    }
    else if (event->key() == Qt::Key_Right) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x + 1,pos.y);
    }
    else if (event->modifiers() == Qt::AltModifier)
    {
        if (event->key() == Qt::Key_X)
        {
            auto position = getNativeMousePos();
            auto color = layerBgImg->pixelColor(position);
            auto rgbStr = QString("rgb(%1,%2,%3)")
                .arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
            QApplication::clipboard()->setText(rgbStr);
        }
        else if (event->key() == Qt::Key_C)
        {
            auto position = getNativeMousePos();
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
    int x1 = maskPath.elementAt(0).x * scaleFactor - maskBorderWidth;
    int y1 = maskPath.elementAt(0).y * scaleFactor - maskBorderWidth;
    int x2 = maskPath.elementAt(2).x * scaleFactor - maskBorderWidth;
    int y2 = maskPath.elementAt(2).y * scaleFactor - maskBorderWidth;
    rect.adjust(x1,y1,x2,y2);
    layerBgImg->copy(rect).save(filePath);
    qApp->exit(0);
}

void MainWindow::saveToClipboard()
{
    endOneDraw();
    QRect rect;
    int x1 = maskPath.elementAt(0).x * scaleFactor - maskBorderWidth;
    int y1 = maskPath.elementAt(0).y * scaleFactor - maskBorderWidth;
    int x2 = maskPath.elementAt(2).x * scaleFactor - maskBorderWidth;
    int y2 = maskPath.elementAt(2).y * scaleFactor - maskBorderWidth;
    rect.adjust(x1,y1,x2,y2);
    auto tempImg = layerBgImg->copy(rect);
    if(tempImg.isNull()){
        QMessageBox::information(this,"info","image is null:"+QString::number(x1)+"_"+QString::number(y1)+"_"+QString::number(x2)+"_"+QString::number(y2),QMessageBox::StandardButton::Ok);
    }
    auto clipboard = QApplication::clipboard();
    qDebug()<<"save to clipboard"<<tempImg.size();
    clipboard->setImage(tempImg);
    auto testImg = clipboard->image();

    //todo 这里是为了兼容老丁的电脑，实际上应该可以不用要，需要再找老丁测试
    if(testImg.isNull()){
        // QMessageBox::information(this,"info","testImg is null:"+QString::number(x1)+"_"+QString::number(y1)+"_"+QString::number(x2)+"_"+QString::number(y2),QMessageBox::StandardButton::Ok);
        QMimeData *data = new QMimeData;
        data->setImageData(tempImg);
        clipboard->setMimeData(data);

        auto testImg2 = clipboard->image();
        if(testImg2.isNull()){
            // QMessageBox::information(this,"info","testImg2 is null:"+QString::number(x1)+"_"+QString::number(y1)+"_"+QString::number(x2)+"_"+QString::number(y2),QMessageBox::StandardButton::Ok);
            clipboard->setPixmap(QPixmap::fromImage(tempImg));
        }
    }

    
    qDebug()<<"saved to clipboard"<<tempImg.size();
    
    QTimer::singleShot(60, this, [this](){
        qApp->exit(0);
    });    
}

