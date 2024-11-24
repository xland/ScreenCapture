#include <QApplication>
#include <format>
#include <Windowsx.h>
#include <dwmapi.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>

#include "WinFull.h"
#include "WinMask.h"
#include "WinCanvas.h"
#include "../App/App.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"

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
    delete winMask;
}
void WinFull::init()
{
    WinFull::dispose();
    winFull = new WinFull();
    winFull->winMask = new WinMask();
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
    auto pos = winMask->maskRect.bottomRight();
    toolMain->move(pos.x() - toolMain->width(), pos.y() + 6);
    toolMain->show();
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        winCanvas = new WinCanvas(this);
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
    painter.drawImage(rect(), img);
    paintShape(&painter);
    qDebug() << "full paint~~~~~";
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
        mouseMoveOnShape(event);
    }
    else {
        winMask->mouseDrag(event);
        mouseDragOnShape(event);
    }
}

void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
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
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            qreal sf = screen->devicePixelRatio();
            img = screen->grabWindow(0, x / sf, y / sf, w / sf, h / sf).toImage();
            break;
        }
    }
}

void WinFull::closeWin()
{
    close();
    if (toolMain) {
        delete toolMain;
        toolMain = nullptr;
    }
    if (toolSub) {
        delete toolSub;
        toolSub = nullptr;
    }
    delete winFull;
    winFull = nullptr;
}
