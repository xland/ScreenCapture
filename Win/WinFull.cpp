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
    initWindow(false);
    show();
}
WinFull::~WinFull()
{
    
}
void WinFull::init()
{
    auto winFull = new WinFull();
    //winFull->winMask = new WinMask(winFull);
    //winFull->pixelInfo = new PixelInfo(winFull);
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
        //winBoard = new WinBoard();
        //winBoard->init(this);
        //winCanvas = new WinCanvas();
        //winCanvas->init(this);
        //winMask->raise();
        //toolMain->raise();
        //toolSub = new ToolSub();
        //toolSub->win = this;
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

void WinFull::close()
{
    if (pixelInfo) {
        //pixelInfo->close();
    }
    if (winMask) {
        delete winMask;
        winMask = nullptr;
    }
    if (winCanvas) {
        delete winCanvas;
        winCanvas = nullptr;
    }
    if (winBoard) {
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
    deleteLater();
}

void WinFull::mousePress(QMouseEvent* event)
{
    event->ignore();
    winMask->mousePress(event);
    mousePressOnShape(event);
}

void WinFull::mousePressRight(QMouseEvent* event)
{
}

void WinFull::mouseDBClick(QMouseEvent* event)
{
}

void WinFull::mouseMove(QMouseEvent* event)
{
    event->ignore();
    //winMask->mouseMove(event);
    if(pixelInfo) pixelInfo->mouseMove(event);
    //mouseMoveOnShape(event);
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
