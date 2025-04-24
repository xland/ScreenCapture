#include <QApplication>
#include <QPainter>

#include "App/App.h"
#include "App/Util.h"
#include "Shape/ShapeBase.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "WinFull.h"
#include "WinPin.h"
#include "CutMask.h"
#include "PixelInfo.h"
#include "Canvas.h"


WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    cutMask = new CutMask(this);
    auto imgBg = Util::printScreen(x,y,w,h);
    imgBg.setDevicePixelRatio(devicePixelRatio());
    canvas = new Canvas(imgBg,this);
    initWindow();
    pixelInfo = new PixelInfo(this);
    pixelInfo->mouseMove(QCursor::pos());
}
WinFull::~WinFull()
{   
}

void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    canvas->paint(p);
    cutMask->paint(p);
}

void WinFull::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (state < State::mask) {
            if(pixelInfo) pixelInfo->close();
            cutMask->mousePress(event);
        }
        else if (state <= State::tool) {
            if(toolMain->isVisible())toolMain->hide();
            cutMask->mousePress(event);
        }
        else {
            canvas->mousePress(event);
        }
    }
}

void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (state <= State::tool) {
            cutMask->mouseDrag(event);
        }
        else {
            canvas->mouseDrag(event);
        }        
    }
    else {
        if (state <= State::tool) {
            cutMask->mouseMove(event);
        }
        else {
            canvas->mouseMove(event);
        }
    }
}

void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
    if (state <= State::tool) {
        state = State::tool;
        if (!toolMain) {
            toolMain = new ToolMain(this);
        }
        toolMain->confirmPos();
        toolMain->show();
    }
    else {
        canvas->mouseRelease(event);
    }
}

void WinFull::pin()
{
    auto img = getTargetImg();
	new WinPin(cutMask->rectMask, img);
    close();
}

void WinFull::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void WinFull::initWindow()
{
    setFocusPolicy(Qt::StrongFocus);
    setGeometry(x, y, w, h);
    setWindowFlag(Qt::FramelessWindowHint);
    show();
    auto hwnd = (HWND)winId();
    SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    SetFocus(hwnd);
}

QImage WinFull::getTargetImg()
{
    auto dpr = devicePixelRatio();
    auto tl = cutMask->rectMask.topLeft() * dpr;
    auto br = cutMask->rectMask.bottomRight() * dpr + QPoint(1, 1);
    QRect r;
    r.setCoords(tl.x(), tl.y(), br.x(), br.y());
    auto img = canvas->imgBg.copy(r);
    auto img2 = canvas->imgBoard.copy(r);
    QPainter p(&img);
    p.drawImage(QPoint(0, 0), img2);
	return img;
}
