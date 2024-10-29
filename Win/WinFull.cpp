
#include <QApplication>
#include <format>
#include <Windowsx.h>
#include <dwmapi.h>
#include <qscreen.h>

#include "WinFull.h"
#include "../App/App.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Layer/CutMask.h"
#include "../Layer/Canvas.h"

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    initSize();
    initScreens();
    initBgImg();
    initWinRects();
    createNativeWindow();
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
}
WinFull::~WinFull()
{
}
void WinFull::init()
{
    WinFull::dispose();
    winFull = new WinFull();
    winFull->processSubWin();
    ShowWindow(winFull->hwnd, SW_SHOW);
    SetForegroundWindow(winFull->hwnd);
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
        toolSub = new ToolSub(this);
        canvas = new Canvas(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    auto pos = toolMain->geometry().bottomLeft();
    toolSub->move(pos.x(), pos.y());
    toolSub->show();
}
void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);
    painter.drawPixmap(rect(),bgImg);
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->state == ShapeState::ready) {
            shapes[i]->paint(&painter);
        }        
    }
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
            if (canvas) {
                canvas->changeShape(nullptr);
            }
            setCursor(Qt::CrossCursor);
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
        canvas->changeShape(nullptr);
    }
}
void WinFull::showEvent(QShowEvent* event)
{
}
void WinFull::closeWin()
{
    close();
    DestroyWindow(hwnd);
    delete winFull;
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
            auto dpr = screen->devicePixelRatio();
            bgImg = screen->grabWindow(0, x / dpr, y / dpr, w / dpr, h / dpr);
            scaleFactor = dpr;
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
void WinFull::createNativeWindow()
{
    auto instance = GetModuleHandle(NULL);
    static int num = 0;
    std::wstring clsName{ std::format(L"ScreenCapture{}", num++) };
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WinFull::routeWinMsg;
    wcx.cbWndExtra = 0;
    wcx.hInstance = instance;
    wcx.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(instance, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcx.lpszClassName = clsName.c_str();
    RegisterClassEx(&wcx);
    auto style{ WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP };
    hwnd = CreateWindowEx(NULL, clsName.c_str(), L"ScreenCapture", style,
        x, y, w, h, NULL, NULL, instance, NULL);
}
void WinFull::processSubWin()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_QuitOnClose, false);
    setFixedSize(w, h);
    show();
    auto widgetHwnd = (HWND)winId();
    SetParent(widgetHwnd, hwnd);
    SetWindowPos(widgetHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
}

LRESULT WinFull::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE: {
            winFull->closeWin();
            break;
        }
        default: {
            break;
        }            
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void WinFull::initWinRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            auto self = (WinFull*)lparam;
            auto sf = self->scaleFactor;
            auto l{ rect.left - self->x }, t{ rect.top - self->y }, r{ rect.right - self->x }, b{ rect.bottom - self->y };
            self->winRects.push_back(QRect(QPoint(l/sf, t/sf), QPoint(r/sf, b/sf)));
            return TRUE;
        }, (LPARAM)this);
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
