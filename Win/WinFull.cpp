#include <QApplication>
#include <QPainter>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPainterPath>
#include <dwmapi.h>

#include "../App/App.h"
#include "../App/NativeRect.h"
#include "../Shape/ShapeBase.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../App/Util.h"
#include "WinFull.h"
#include "WinPin.h"
#include "CutMask.h"
#include "PixelInfo.h"

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    cutMask = new CutMask(this);
    initWindow();
    auto dpr = devicePixelRatio();
    imgBg = Util::printScreen();
    imgBg.setDevicePixelRatio(dpr);
    imgBoard = imgBg.copy();
    imgCanvas = imgBg.copy();
    pixelInfo = new PixelInfo(this);
    pixelInfo->mouseMove(QCursor::pos());
}
WinFull::~WinFull()
{   
}
void WinFull::init()
{
    winFull = new WinFull();
}

void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawImage(0, 0, imgBg);
    p.drawImage(0, 0, imgBoard);
    p.drawImage(0, 0, imgCanvas);
    cutMask->paint(p);
}

void WinFull::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (toolMain) toolMain->hide();
        if (state < State::mask && pixelInfo) pixelInfo->close();
        cutMask->mousePress(event);
    }
}

void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {        
        if (toolMain && toolMain->isVisible()) {
            toolMain->hide();
        }
        if (toolSub && toolSub->isVisible()) {
            toolSub->hide();
        }
        cutMask->mouseDrag(event);
    }
    else {
        cutMask->mouseMove(event);
    }
}

void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
    state = State::tool;    
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    toolMain->confirmPos();
    toolMain->show();
}

void WinFull::close()
{
    deleteLater();
    winFull = nullptr;
}