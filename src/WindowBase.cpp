#include "WindowBase.h"
#include <dwmapi.h>
#include "include/effects/SkRuntimeEffect.h"
#include "Util.h"

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    delete[] pixelFront;
    delete[] pixelBack;
    delete[] pixelBase;
    DeleteDC(hCompatibleDC);
    DeleteObject(bottomHbitmap);
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
    SetCursor(hCursor);
    Refresh();
}

void WindowBase::Refresh()
{
    static size_t size = w * 4 * h;
    memcpy(pixelBack, pixelBase, size);
    auto back = surfaceBack->getCanvas();
    auto front = surfaceFront->getCanvas();
    front->clear(SK_ColorTRANSPARENT);
    //front->saveLayer(nullptr,nullptr);
    paint(front);
    //front->restore();
    sk_sp<SkImage> img = surfaceFront->makeImageSnapshot();
    back->drawImage(img, 0, 0);
    HDC hdc = GetDC(hwnd);
    static BITMAPINFO info = {sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0};
    SetDIBits(hdc, bottomHbitmap, 0, h, pixelBack, &info, DIB_RGB_COLORS);
    static BLENDFUNCTION blend = {.BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA}};
    static POINT pSrc = {0, 0};
    static SIZE sizeWnd = {w, h};
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}

void WindowBase::initCanvas()
{
    HDC hdc = GetDC(hwnd);
    hCompatibleDC = CreateCompatibleDC(NULL);
    bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
    ReleaseDC(hwnd, hdc);
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    size_t stride = w * 4;
    size_t size = stride * h;
    pixelBack = new unsigned char[size];
    surfaceBack = SkSurface::MakeRasterDirect(info, pixelBack, stride);
    pixelFront = new unsigned char[size];
    surfaceFront = SkSurface::MakeRasterDirect(info, pixelFront, stride);
}

void WindowBase::Close(const int &exitCode)
{
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
    PostQuitMessage(0);
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
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_ESCAPE:
            {
                obj->Close(3);
                return false;
            }
            }
            return false;
        }
        case WM_KEYUP:
        {
            return false;
        }
        default:
        {
            return obj->WindowProc(hWnd, msg, wParam, lParam);
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