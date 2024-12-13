#include <QApplication>
#include <Windows.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>

#include "WinFull.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "../App/App.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"

WinFull::WinFull(QObject* parent) : WinBox(parent)
{
    initWinSizeByDesktopSize();
    //initDesktopImg();
    initDesktopImgNative();
    initWindow(false);
    show();
}
WinFull::~WinFull()
{
    
}
void WinFull::init()
{
    auto winFull = new WinFull();
    winFull->winMask = new WinMask(winFull);
    winFull->pixelInfo = new PixelInfo(winFull);
}
void WinFull::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    toolMain->show();
    toolMain->raise();  
    //todo 确认会不会闪一下
    auto pos = winMask->maskRect.bottomRight();
    pos.setX(pos.x() + x);
    pos.setY(pos.y() + y);
    auto hwnd = (HWND)toolMain->winId();
    auto dpr = toolMain->windowHandle()->devicePixelRatio();
    SetWindowPos(hwnd, nullptr,pos.x() - toolMain->width()*dpr, pos.y() + 6*dpr, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        winBoard = new WinBoard(this);
        winCanvas = new WinCanvas(this);
        winMask->raise();  //todo 确认会不会闪一下
        toolMain->raise();
        toolSub = new ToolSub(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
}

void WinFull::initWinSizeByDesktopSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void WinFull::initDesktopImg()
{
    //需要提前拍照，不然把自己也拍进来了
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            dpr = screen->devicePixelRatio();
            img = screen->grabWindow(0, x / dpr, y / dpr, w / dpr, h / dpr)
                .toImage().convertToFormat(QImage::Format_ARGB32);
            break;
        }
    }
}

void WinFull::initDesktopImgNative()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    img = QImage(w, h, QImage::Format_ARGB32);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)w * 4*h, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, img.bits(), &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
}



void WinFull::close()
{
    if (pixelInfo) {
        pixelInfo->close();
    }
    if (winMask) {
        winMask->close();
    }
    if (winCanvas) {
        winCanvas->close();
    }
    if (winBoard) {
        winBoard->close();
    }
    if (toolMain) {
        toolMain->close();
    }
    if (toolSub) {
        toolSub->close();
    }
    WinBase::close();
}

void WinFull::mousePress(QMouseEvent* event)
{
    event->ignore();
    winMask->mousePress(event);
    mousePressOnShape(event);
}

void WinFull::mouseMove(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseMove(event);
    if (pixelInfo && !event->isAccepted()) {
        pixelInfo->mouseMove(event);
    }
    if (!event->isAccepted()) {
        mouseMoveOnShape(event);
    }
    if (!event->isAccepted()) {
        QGuiApplication::setOverrideCursor(Qt::CrossCursor);
    }
}

void WinFull::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseDrag(event);
    mouseDragOnShape(event);
}

void WinFull::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseRelease(event);
    mouseReleaseOnShape(event);
}
