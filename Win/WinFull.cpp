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

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    initWinSizeByDesktopSize();
    initDesktopImg();
    initWindow();
}
WinFull::~WinFull()
{
    
}
void WinFull::init()
{
    WinFull::dispose();
    winFull = new WinFull();
    winFull->winMask = new WinMask();
    winFull->winMask->init(winFull);
    winFull->winMask->initWinRects();
    winFull->pixelInfo = new PixelInfo();
}
void WinFull::dispose()
{
    if (winFull) {
        winFull->closeWin();
    }
}
WinFull* WinFull::get()
{
    return winFull;
}
void WinFull::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain();
        toolMain->win = this;
    }
    toolMain->show();
    toolMain->raise();
    auto pos = winMask->maskRect.bottomRight();
    pos.setX(pos.x() + x);
    pos.setY(pos.y() + y);
    auto hwnd = (HWND)toolMain->winId();
    SetWindowPos(hwnd, nullptr,pos.x() - toolMain->width()*dpr, pos.y() + 6*dpr, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        winBoard = new WinBoard();
        winBoard->init(this);
        winCanvas = new WinCanvas();
        winCanvas->init(this);
        winMask->raise();
        toolMain->raise();
        toolSub = new ToolSub();
        toolSub->win = this;
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
}
void WinFull::paintEvent(QPaintEvent* event)
{    
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), img);
}

void WinFull::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
    winMask->mousePress(event);
    mousePressOnShape(event);
}

void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
    if (event->buttons() == Qt::NoButton) {
        winMask->mouseMove(event);
        pixelInfo->mouseMove(event);
        mouseMoveOnShape(event);
    }
    else {
        winMask->mouseDrag(event);
        mouseDragOnShape(event);
    }
}

void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseRelease(event);
    mouseReleaseOnShape(event);
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

void WinFull::closeWin()
{
    close();
    if (pixelInfo) {
        pixelInfo->close();
        delete pixelInfo;
        pixelInfo = nullptr;
    }
    if (winMask) {
        winMask->close();
        delete winMask;
        winMask = nullptr;
    }
    if (winCanvas) {
        winCanvas->close();
        delete winCanvas;
        winCanvas = nullptr;
    }
    if (winBoard) {
        winBoard->close();
        delete winBoard;
        winBoard = nullptr;
    }
    if (toolMain) {
        toolMain->close();
        delete toolMain;
        toolMain = nullptr;
    }
    if (toolSub) {
        toolSub->close();
        delete toolSub;
        toolSub = nullptr;
    }
    delete winFull;
    winFull = nullptr;
}
