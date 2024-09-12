
#include <format>


#include "WindowNative.h"
#include "CanvasWidget.h"

namespace {
    std::unique_ptr<WindowNative> win;
}

void WindowNative::Init()
{
    win = std::make_unique<WindowNative>();
    win->initPosSize();
    win->getScreenRects();
    win->shotScreen();
    win->createWindow();
}
WindowNative* WindowNative::Get()
{
    return win.get();
}
void WindowNative::createWindow()
{
    auto instance = GetModuleHandle(NULL);
    static int num = 0;
    std::wstring clsName{ std::format(L"ScreenCapture{}", num++) };
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowNative::routeWinMsg;
    wcx.cbWndExtra = sizeof(WindowNative*);
    wcx.hInstance = instance;
    wcx.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(instance, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcx.lpszClassName = clsName.c_str();
    RegisterClassEx(&wcx);
    hwnd = CreateWindowEx(NULL, clsName.c_str(), L"ScreenCapture", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
        x, y, w, h, NULL, NULL, instance, static_cast<LPVOID>(this));

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    auto canvas = new CanvasWidget();
    auto qtHwnd = (HWND)canvas->winId();
    SetParent(qtHwnd, hwnd);
    SetWindowPos(qtHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
LRESULT WindowNative::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else if (msg == WM_SETCURSOR) {
        return true;
    }
    else if (msg == WM_SETTINGCHANGE) {
        return true;
    }
    //else if (msg == WM_ERASEBKGND) {
    //    return true;
    //}
    auto obj = reinterpret_cast<WindowNative*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return obj->processNativeMsg(hWnd, msg, wParam, lParam);
}

LRESULT WindowNative::processNativeMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


void WindowNative::shotScreen()
{
    HDC hScreen = GetDC(NULL); // 获取整个屏幕的设备上下文
    HDC hDC = CreateCompatibleDC(hScreen); // 创建内存设备上下文
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // 创建一个与屏幕设备上下文兼容的位图
    DeleteObject(SelectObject(hDC, hBitmap));  // 选择该位图到内存设备上下文中
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY); //将屏幕内容复制到位图
    long long dataSize = w * h * 4;
    winPix.resize(dataSize);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, &winPix.front(), &info, DIB_RGB_COLORS); //获取位图的像素数据
    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
}

void WindowNative::getScreenRects()
{
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            WindowNative* self = reinterpret_cast<WindowNative*>(lParam);
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &info);
            self->screenRects.push_back(info.rcMonitor);
            return TRUE;
        }, reinterpret_cast<LPARAM>(this));
}

void WindowNative::initPosSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
