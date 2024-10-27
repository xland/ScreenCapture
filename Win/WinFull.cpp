
#include <QApplication>
#include <format>
#include <Windowsx.h>
#include <dwmapi.h>
#include <qscreen.h>

#include "../App/App.h"
#include "WinFull.h"
#include "../Layer/LayerBoard.h"
#include "../Layer/LayerMask.h"
#include "../Layer/LayerCanvas.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : QWidget(parent)
{
    initSize();
    initScreens();
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
    winFull->initBgImg();
    winFull->initLayers();
    winFull->processWidget();
    ShowWindow(winFull->hwnd, SW_SHOW);
    SetForegroundWindow(winFull->hwnd);
}
void WinFull::dispose()
{
    if (winFull) {
        winFull->close();
    }
}
WinFull* WinFull::get()
{
    return winFull;
}
void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    layerBoard->paint(&painter);
    layerCanvas->paint(&painter);
    layerMask->paint(&painter);
}
void WinFull::mousePressEvent(QMouseEvent* event)
{
}
void WinFull::mouseMoveEvent(QMouseEvent* event)
{
}
void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
}
void WinFull::showEvent(QShowEvent* event)
{
}
void WinFull::closeNative()
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
            winFull->bgImg = screen->grabWindow(0, x / dpr, y / dpr, w / dpr, h / dpr).toImage();
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
void WinFull::initLayers()
{
    layerBoard = new LayerBoard(this);
    layerCanvas = new LayerCanvas(this);
    layerMask = new LayerMask(this);
}
void WinFull::createTool()
{
    //toolMain = new ToolMain();
    //toolSub = new ToolSub();
    //processTool(toolMain);
    //processTool(toolSub);
}
void WinFull::processWidget()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //tar->setAttribute(Qt::WA_Hover);
    //tar->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    setFixedSize(w, h);
    show();

    auto widgetHwnd = (HWND)winId();
    SetParent(widgetHwnd, hwnd);
    SetWindowPos(widgetHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
void WinFull::processTool(QWidget* tar)
{
    tar->setAttribute(Qt::WA_QuitOnClose, false);
    tar->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    tar->setAttribute(Qt::WA_TranslucentBackground);
    tar->setAttribute(Qt::WA_NoSystemBackground);
    tar->setMouseTracking(true);
    tar->setVisible(false);
    tar->setAttribute(Qt::WA_Hover);  //enterEvent 和 leaveEvent，以及 hoverMoveEvent
}

LRESULT WinFull::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        //case WM_MOUSEMOVE: {
        //    int x = GET_X_LPARAM(lParam);
        //    int y = GET_Y_LPARAM(lParam);
        //    qDebug() << "allen" << x << y;
        //    break;
        //}
        case WM_CLOSE: {
            winFull->closeNative();
            break;
        }
        default: {
            break;
        }            
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
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
