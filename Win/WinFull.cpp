#include <QApplication>
#include <format>
#include <Windowsx.h>
#include <dwmapi.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>

#include "WinFull.h"
#include "../App/App.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Layer/CutMask.h"
#include "../Layer/Canvas.h"
#include "../Layer/Board.h"

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    initSize();
    initBgImg();
        
    setFixedSize(w, h);
    show();
    SetWindowPos((HWND)winId(), nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
    setFocus();
}
WinFull::~WinFull()
{
}
void WinFull::init()
{
    WinFull::dispose();
    winFull = new WinFull();
    winFull->initScreens();
    winFull->cutMask = new CutMask(winFull);
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
        toolMain = new ToolMain(this);
    }
    auto pos = cutMask->maskRect.bottomRight();
    toolMain->move(pos.x() - toolMain->width(), pos.y() + 6);
    toolMain->show();
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        canvas = new Canvas(this);
        board = new Board(this);
        toolSub = new ToolSub(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
}
void WinFull::paintEvent(QPaintEvent* event)
{    
    QWindow* window = windowHandle();
    auto sf = window->devicePixelRatio();
    QPainter painter(this);    
    painter.drawPixmap(rect(), bgImg);
}

void WinFull::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
    cutMask->mousePress(event);
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mousePress(event);
    }
    if (!event->isAccepted()) {
        auto shape = addShape();
        shape->mousePress(event); //不然新添加的Shape收不到鼠标按下事件
    }
}
void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
    if (event->buttons() == Qt::NoButton) {
        cutMask->mouseMove(event);
        if (event->isAccepted()) return;
        for (int i = shapes.size() - 1; i >= 0; i--)
        {
            if (event->isAccepted()) return;
            shapes[i]->mouseMove(event);
        }
        if (!event->isAccepted()) {
            if (state == State::text) {
                winFull->updateCursor(Qt::IBeamCursor);
            }
            else {
                winFull->updateCursor(Qt::CrossCursor);
            }
            if (canvas) {
                canvas->changeShape(nullptr);
            }
        }
    }
    else {
        cutMask->mouseDrag(event);
        if (event->isAccepted()) return;
        for (int i = shapes.size() - 1; i >= 0; i--)
        {
            if (event->isAccepted()) return;
            shapes[i]->mouseDrag(event);
        }
    }
}
void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
    cutMask->mouseRelease(event);
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseRelease(event);
    }
    if (canvas && !event->isAccepted()) {
        if (state != State::text) {
            canvas->changeShape(nullptr);
        }
    }
}
void WinFull::closeWin()
{
    close();
    delete cutMask;
    cutMask = nullptr;
    delete winFull;
    winFull = nullptr;
}

void WinFull::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
void WinFull::initBgImg()
{
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            qreal sf = screen->devicePixelRatio();
            bgImg = screen->grabWindow(0, x / sf, y / sf, w / sf, h / sf);
        }
    }
}

void WinFull::initScreens() {
    //EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
    //    {
    //        MONITORINFO info;
    //        info.cbSize = sizeof(MONITORINFO);
    //        GetMonitorInfo(hMonitor, &info);
    //        POINT leftTop{ .x{info.rcMonitor.left},.y{info.rcMonitor.top} };
    //        POINT rightBottom{ .x{info.rcMonitor.right},.y{info.rcMonitor.bottom} };
    //        auto full = (WinFull*)lParam;
    //        ScreenToClient(full->hwnd, &leftTop);
    //        ScreenToClient(full->hwnd, &rightBottom);
    //        full->screens.push_back(QRect(leftTop.x, leftTop.y, rightBottom.x + 1, rightBottom.y + 1));
    //        return TRUE;
    //    }, (LPARAM)this);
}
