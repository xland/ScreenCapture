#include <qlayout.h>
#include <qpushbutton.h>
#include <QEvent>

#include "CanvasWidget.h"
#include "CutMask.h"
#include "WindowNative.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"

namespace
{
CanvasWidget* canvasWidget;
}

CanvasWidget::CanvasWidget(QWidget* parent) : QWidget(parent)
{
    initImgs();
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_QuitOnClose, false);
    setMouseTracking(true);
    auto winNative = WindowNative::Get();
    setFixedSize(winNative->w, winNative->h);
    cutMask = new CutMask(this);
    toolMain = new ToolMain(this);
    toolSub = new ToolSub(this);
    show();
}

CanvasWidget::~CanvasWidget()
{
    canvasWidget = nullptr;
}

void CanvasWidget::Init()
{
    canvasWidget = new CanvasWidget();
    auto hwnd = (HWND)canvasWidget->winId();
    auto winNative = WindowNative::Get();
    SetParent(hwnd, winNative->hwnd);
    SetWindowPos(hwnd, nullptr, 0, 0, winNative->w, winNative->h, SWP_NOZORDER | SWP_SHOWWINDOW);
    ShowWindow(winNative->hwnd, SW_SHOW);
    SetForegroundWindow(winNative->hwnd);
}

CanvasWidget* CanvasWidget::Get()
{
    return canvasWidget;
}

void CanvasWidget::addShape()
{
    shapes.erase(std::remove_if(shapes.begin(), shapes.end(), [](ShapeBase * item) { return item->state == ShapeState::temp; }), shapes.end());
    toolSub->hide();
    if (state == State::rect)
    {
        shapes.push_back(new ShapeRect(this));
    }
    else if (state == State::ellipse)
    {
        shapes.push_back(new ShapeEllipse(this));
    }
    cutMask->raise();
    toolMain->raise();
    toolSub->raise();
    toolSub->show();
}

void CanvasWidget::dispathcEvent(QMouseEvent* event)
{
    for (auto shape : shapes)
    {
        QCoreApplication::sendEvent(shape, event);
    }
    QCoreApplication::sendEvent(this, event);
}


void CanvasWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawImage(rect(), imgBg);
    // painter.drawPixmap(rect(), desktopImg);
}

void CanvasWidget::closeEvent(QCloseEvent* event)
{
    delete canvasWidget;
}

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{

}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    addShape();
}

void CanvasWidget::initImgs()
{
    auto screenRect = QRect(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
            GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            auto dpr = screen->devicePixelRatio();
            imgBg = screen->grabWindow(0,
                    screenRect.x() / dpr,
                    screenRect.y() / dpr,
                    screenRect.width() / dpr,
                    screenRect.height() / dpr).toImage();
            break;
        }
    }

    //auto winNative = WindowNative::Get();
    //HDC hScreen = GetDC(NULL);
    //HDC hDC = CreateCompatibleDC(hScreen);
    //HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, winNative->w, winNative->h);
    //DeleteObject(SelectObject(hDC, hBitmap));
    //BOOL bRet = BitBlt(hDC, 0, 0, winNative->w, winNative->h, hScreen, winNative->x, winNative->y, SRCCOPY);
    //long long dataSize = winNative->w * winNative->h * 4;
    //std::vector<unsigned char> bgPix(dataSize, 0);
    //BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)winNative->w, 0 - (long)winNative->h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
    //GetDIBits(hDC, hBitmap, 0, winNative->h, &bgPix.front(), &info, DIB_RGB_COLORS);
    //DeleteObject(hBitmap);
    //DeleteDC(hDC);
    //ReleaseDC(NULL, hScreen);
    //QImage bgTemp(&bgPix.front(), winNative->w, winNative->h, QImage::Format_ARGB32);
    ////auto bg = bgTemp.convertToFormat(QImage::Format_RGB444); //QImage::Format_RGB444 让图像体积小一倍，但图像颜色会变得不一样
    ////imgBg = std::make_unique<QImage>(std::move(bgTemp));
    //imgBg = std::make_unique<QImage>(bgTemp.copy(0, 0, winNative->w, winNative->h));

}
