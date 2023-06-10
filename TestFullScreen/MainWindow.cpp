#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <Windows.h>
#include <shellscalingapi.h>
#include "QTimer"
#include <QImage>
#include <QPainter>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    screenRect = QRect(GetSystemMetrics(SM_XVIRTUALSCREEN),
                       GetSystemMetrics(SM_YVIRTUALSCREEN),
                       GetSystemMetrics(SM_CXVIRTUALSCREEN),
                       GetSystemMetrics(SM_CYVIRTUALSCREEN));
    this->shotScreen();

    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    QTimer::singleShot(0, this, [this](){
        HWND hwnd = (HWND)this->winId();
        SetWindowPos(hwnd,HWND_TOP,
                     this->screenRect.x(),
                     this->screenRect.y(),
                     this->screenRect.width(),
                     this->screenRect.height(),
                     SWP_HIDEWINDOW);
        SetWindowPos(hwnd,HWND_TOP,
                     this->screenRect.x(),
                     this->screenRect.y(),
                     this->screenRect.width(),
                     this->screenRect.height(),
                     SWP_SHOWWINDOW);
            int dpi = GetDpiForWindow(hwnd);
            qreal rate;
            switch (dpi) {
            case 96:
                    rate = 1.0;
                break;
            case 120:
                    rate = 1.25;
                break;
            case 144:
                    rate = 1.5;
                break;
            case 168:
                rate = 1.75;
                break;
            case 192:
                rate = 2.0;
                break;
            default:
                rate = 1.0;
                break;
            }
            qDebug()<<rate;
            bgImg.setDevicePixelRatio(rate);
    });
    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawImage(0,0, bgImg);
}

void  MainWindow::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenRect.width(), screenRect.height());
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    if (!result) {
    }
    result = BitBlt(hDC, 0, 0, screenRect.width(), screenRect.height(), hScreen, screenRect.x(), screenRect.y(), SRCCOPY);
    if (!result) {
        }



//qDebug() << "rate" << rate;
//    qDebug() << "screen size" << screenRect.width()<<screenRect.height();
    bgImg = QImage::fromHBITMAP(hBitmap);
//    bgImg.save("bgImg.png");
//    auto screens = QGuiApplication::screens();
//    for (int var = 0; var < screens.count(); ++var) {
//        qDebug()<<screens[var]->virtualSize();
//        qDebug()<<screens[var]->size();
//        qDebug()<<screens[var]->physicalDotsPerInch();
//    }
//    qreal tempSizeMax = 0.0;
//    qreal tempSizeMin = 99999999.0;
//    qreal rateMax = 0.0;
//    qreal rateMin = 0.0;
//    for (int var = 0; var < screens.count(); ++var) {
//        auto tempRate = screens[var]->devicePixelRatio();
//        auto sizeCur = screens[var]->size()*tempRate;
//        auto sizeCurVal = sizeCur.width()*sizeCur.height();
//        if(sizeCurVal > tempSizeMax){
//            tempSizeMax = sizeCurVal;
//            rateMax = tempRate;
//        }
//        if(sizeCurVal < tempSizeMin){
//            tempSizeMin = sizeCurVal;
//            rateMin = tempRate;
//        }
//    }
//    auto rate = screens[0]->devicePixelRatio();
//    if(rate != rateMax){
//        bgImg.setDevicePixelRatio(rateMax);
//    }else{
//        bgImg.setDevicePixelRatio(rateMin);
//    }

//    qreal rateBig = 1.0;
//    qreal rateSmall = 999.0;
//    qreal rateImpornt = screens[0]->devicePixelRatio();
//    bool isAllScreenSame = true;
//    bool isPrimaryBig = true;
//    QSize tempSize = screens[0]->size()*screens[0]->devicePixelRatio();
//    for (int var = 0; var < screens.count(); ++var) {
//        auto rate = screens[var]->devicePixelRatio();
//        if(rateBig <rate){
//            rateBig = rate;
//        }
//        if(rateSmall > rate){
//            rateSmall = rate;
//        }
//        if(var > 0){
//            auto sizeCur = screens[var]->size()*rate;
//            if(tempSize != sizeCur){
//                isAllScreenSame = false;
//                auto curVal = sizeCur.width()*sizeCur.height();
//                auto tempVal = tempSize.width()*tempSize.height();
//                if(curVal > tempVal){
//                    tempSize = sizeCur;
//                    rateImpornt = rate;
//                    isPrimaryBig = false;
//                }else{

//                }
//            }
//        }
//    }
//    if(isAllScreenSame){
//        bgImg.setDevicePixelRatio(rateSmall);
//    }else{
//        bgImg.setDevicePixelRatio(rateImpornt);
//    }
    bgImg.reinterpretAsFormat(QImage::QImage::Format_ARGB32_Premultiplied);
    result = DeleteDC(hDC);
    if (!result) {
    }
    ReleaseDC(NULL, hScreen);
    result = DeleteObject(hBitmap);
    if (!result) {
    }


//    result = BitBlt(hDC, 0, 0, screenRect.width(), screenRect.height(), hScreen, screenRect.x(), screenRect.y(), SRCCOPY);
//    if (!result) {
//    }
//    unsigned int dataSize = ((screenRect.width() * 32 + 31) / 32) * 4 * screenRect.height();
//    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER),
//                        static_cast<long>(screenRect.width()),
//                        static_cast<long>(0 - screenRect.height()),
//                        1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
//    std::vector<std::uint8_t> pixels;
//    int r = GetDIBits(hDC, hBitmap, 0, screenRect.height(), pixels.data(), &Info, DIB_RGB_COLORS);
//    if (r == 0) {
//    }
}

