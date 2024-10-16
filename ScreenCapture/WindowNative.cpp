
#include <format>
#include <QApplication>
#include "WindowNative.h"
#include "CanvasWidget.h"

namespace {
    WindowNative* win;
}

void WindowNative::Init()
{
    win = new WindowNative();
    win->createWindow();
}
WindowNative* WindowNative::Get()
{
    return win;
}
void WindowNative::Close()
{
    //CloseWindow(win->hwnd);
    DestroyWindow(win->hwnd);
    win = nullptr;
}
void WindowNative::createWindow()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    auto instance = GetModuleHandle(NULL);
    static int num = 0;
    std::wstring clsName{ std::format(L"ScreenCapture{}", num++) };
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowNative::routeWinMsg;
    wcx.cbWndExtra = 0;
    wcx.hInstance = instance;
    wcx.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(instance, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcx.lpszClassName = clsName.c_str();
    RegisterClassEx(&wcx);
    hwnd = CreateWindowEx(NULL, clsName.c_str(), L"ScreenCapture", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
        x, y, w, h, NULL, NULL, instance, NULL);
}
LRESULT WindowNative::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE: {
            DestroyWindow(hWnd);
            win = nullptr;
            //PostQuitMessage(0);
            return true;
        }
        default: {
            break;
        }            
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
