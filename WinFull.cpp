
#include <QApplication>
#include <format>
#include <Windowsx.h>

#include "App.h"
#include "WinFull.h"
#include "WinBoard.h"
#include "WinMask.h"
#include "WinCanvas.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"

WinFull::WinFull(QObject* parent) : QObject(parent)
{
    initSize();
    createNativeWindow();
    createWidget();
    createTool();
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
}
WinFull::~WinFull()
{
    delete toolMain;
    delete toolSub;
    delete board;
    delete canvas;
    delete mask;
}
void WinFull::close()
{
    toolMain->close();
    toolSub->close();
    board->close();
    canvas->close();
    mask->close();
    DestroyWindow(hwnd);
    App::disposeFull();
}

void WinFull::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
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
void WinFull::createWidget()
{
    board = new WinBoard();
    processWidget(board);
    canvas = new WinCanvas();
    processWidget(canvas);
    mask = new WinMask();
    processWidget(mask);
    connect(board, &WinBoard::mouseDrag, mask, &WinMask::mouseDrag);
    connect(board, &WinBoard::mouseMove, mask, &WinMask::mouseMove);
    connect(board, &WinBoard::mousePress, mask, &WinMask::mousePress);
    connect(board, &WinBoard::mouseRelease, mask, &WinMask::mouseRelease);
}
void WinFull::createTool()
{
    toolMain = new ToolMain();
    toolSub = new ToolSub();
    processTool(toolMain);
    processTool(toolSub);
}
void WinFull::processWidget(QWidget* tar)
{
    tar->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    tar->setAttribute(Qt::WA_TranslucentBackground);
    //tar->setAttribute(Qt::WA_Hover);
    //tar->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    tar->setAttribute(Qt::WA_NoSystemBackground, true);
    tar->setAttribute(Qt::WA_QuitOnClose, false);
    tar->setFixedSize(w, h);
    tar->show();

    auto widgetHwnd = (HWND)tar->winId();
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
    //switch (msg)
    //{
    //    //case WM_MOUSEMOVE: {
    //    //    int x = GET_X_LPARAM(lParam);
    //    //    int y = GET_Y_LPARAM(lParam);
    //    //    qDebug() << "allen" << x << y;
    //    //    break;
    //    //}
    //    default: {
    //        break;
    //    }            
    //}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
