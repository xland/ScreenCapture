#include <Windows.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qscreen.h>
#include <QColor>
#include <qpushbutton.h>

#include "WindowMain.h"
#include "WindowNative.h"


WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    QPushButton* button = new QPushButton("Click Me", this);
    setCentralWidget(button);

    // 将 QMainWindow 转换为 QWindow
    QWindow* window = windowHandle();

    // 创建一个窗口容器并嵌入到原生窗口中
    QWidget* container = QWidget::createWindowContainer(window);
    container->setWindowFlags(Qt::FramelessWindowHint);
    container->setParent(nullptr);
    auto native = WindowNative::Get();
    container->setGeometry(0, 0, native->w, native->h);
    SetParent((HWND)container->winId(), native->hwnd);
    container->show();


    //setWindowFlags(Qt::FramelessWindowHint);
    //initPosSize();
    //shotScreen();
    //auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    //setGeometry(x, y, w/scaleFactor, h / scaleFactor);

    //setGeometry(x, y, w, h);
    //setFixedSize(w, h);
    ui.setupUi(this);
}

WindowMain::~WindowMain()
{
    delete bgImg;
}

void WindowMain::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::GlobalColor::red);
    painter.drawLine(0, 0, this->width(), this->height());
}

//void WindowMain::shotScreen()
//{
//    HDC hScreen = GetDC(NULL); // 获取整个屏幕的设备上下文
//    HDC hDC = CreateCompatibleDC(hScreen); // 创建内存设备上下文
//    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // 创建一个与屏幕设备上下文兼容的位图
//    DeleteObject(SelectObject(hDC, hBitmap));  // 选择该位图到内存设备上下文中
//    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY); //将屏幕内容复制到位图
//    long long dataSize = w * h * 4;
//    bgPix.resize(dataSize);
//    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
//    GetDIBits(hDC, hBitmap, 0, h, &bgPix.front(), &info, DIB_RGB_COLORS); //获取位图的像素数据
//    DeleteObject(hBitmap);
//    DeleteDC(hDC);
//    ReleaseDC(NULL, hScreen);
//    bgImg = new QImage(&bgPix.front(), w, h, QImage::Format_ARGB32_Premultiplied);
//}
//void WindowMain::initPosSize()
//{
//    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
//    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
//    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//}
