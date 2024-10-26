#include <qlayout.h>
#include <qpushbutton.h>
#include <QEvent>

#include "App.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "WinMask.h"
#include "WinFull.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"

WinBoard::WinBoard(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    initImgs();  
}

WinBoard::~WinBoard()
{
}

void WinBoard::addShape(const QPoint& pos)
{
    for (auto it = shapes.begin(); it != shapes.end(); ) {
        if ((*it)->state == ShapeState::temp) {
            it = shapes.erase(it);
        }
        else {
            ++it;
        }
    }
    ShapeBase* shape;
    auto full = App::getFull();
    if (full->state == State::rect) {
        shape = new ShapeRect(this);
    }
    else if (full->state == State::ellipse) {
        shape = new ShapeEllipse(this);
    }
    else
    {
        return;
    }
    connect(shape, &ShapeBase::onHover, App::getFullCanvas(), &WinCanvas::onShapeHover);
    shapes.push_back(shape);
}

void WinBoard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);
    // painter.drawImage(rect(), imgBg);
    painter.drawPixmap(rect(), desktopImg);
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->state != ShapeState::ready) {
            continue;
        }
        shapes[i]->paint(&painter);
    }
}

void WinBoard::mousePressEvent(QMouseEvent* event)
{
    //lower(); //不如native event fillter
    event->ignore();
    emit mousePress(event);
    if (!event->isAccepted()) {
        addShape(event->pos());
        emit mousePress(event); //不然新添加的Shape收不到鼠标按下事件
    }
}

void WinBoard::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
    if (event->buttons() == Qt::NoButton) {
        emit mouseMove(event);
    }
    else {
        emit mouseDrag(event);
    }
}

void WinBoard::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
    emit mouseRelease(event);
}

void WinBoard::showEvent(QShowEvent* event)
{
    hwnd = (HWND)winId();
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
}

void WinBoard::initImgs()
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
            desktopImg = screen->grabWindow(0,
                    screenRect.x() / dpr,
                    screenRect.y() / dpr,
                    screenRect.width() / dpr,
                    screenRect.height() / dpr);
            break;
        }
    }

    //auto winNative = WinFull::Get();
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


