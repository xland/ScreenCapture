#include "WindowBase.h"
#include <windowsx.h>
#include <dwmapi.h>
#include "include/effects/SkRuntimeEffect.h"
#include "Util.h"

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    delete[] pixelData;
    DeleteDC(hCompatibleDC);
    DeleteObject(bottomHbitmap);
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

void WindowBase::Refresh()
{
    auto canvas = surface->getCanvas();
    paint(canvas);
    HDC hdc = GetDC(hwnd);
    BITMAPINFO info = {sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0};
    SetDIBits(hdc, bottomHbitmap, 0, h, pixelData, &info, DIB_RGB_COLORS);
    BLENDFUNCTION blend = {.BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA}};
    POINT pSrc = {0, 0};
    SIZE sizeWnd = {w, h};
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}

void WindowBase::initCanvas()
{
    HDC hdc = GetDC(hwnd);
    hCompatibleDC = CreateCompatibleDC(NULL);
    bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    pixelData = new unsigned char[w * h * 4];
    surface = SkSurface::MakeRasterDirect(info, pixelData, w * 4);
    ReleaseDC(hwnd, hdc);
}
LRESULT CALLBACK WindowBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *pCS = reinterpret_cast<CREATESTRUCT *>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WindowBase *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj)
    {
        switch (msg)
        {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                return false;
            }
            break;
        }
        case WM_SETCURSOR:
        {
            return true;
        }
        default:
        {
            if (obj->pixelData)
            {
                return obj->WindowProc(hWnd, msg, wParam, lParam);
            }
        }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void WindowBase::initWindow()
{
    static int num = 0;
    std::wstring className = std::format(L"ScreenCapture{}", num++);
    auto hinstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowBase::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(WindowBase *);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = className.c_str();
    if (!RegisterClassEx(&wcx))
    {
        return;
    }
    auto exStyle = WS_EX_LAYERED;
    this->hwnd = CreateWindowEx(exStyle, wcx.lpszClassName, wcx.lpszClassName, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
                                x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}