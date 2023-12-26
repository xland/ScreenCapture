#include "WindowBase.h"
#include <dwmapi.h>
#include "Util.h"
#include "include/core/SkPath.h"

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    delete[] pixBase->addr();
    delete pixBase;
    delete pixBack;
    DeleteDC(hCompatibleDC);
    DeleteObject(bottomHbitmap);
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    Refresh();
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

void WindowBase::Refresh()
{
    surfaceBack->writePixels(*pixBase, 0, 0);
    auto back = surfaceBack->getCanvas();
    auto front = surfaceFront->getCanvas();
    front->clear(SK_ColorTRANSPARENT);
    paint(front);
    sk_sp<SkImage> img = surfaceFront->makeImageSnapshot();
    back->drawImage(img,0,0);
    paintFinish(back);

    //back->saveLayer(nullptr, nullptr);
    //SkPaint paint;
    //paint.setColor(SK_ColorBLUE);
    //back->drawRect(SkRect::MakeXYWH(0, 0, 800, 600),paint);
    //SkPath path;
    //path.moveTo(0, 0);
    //path.lineTo(400, 300);
    //path.setFillType(SkPathFillType::kInverseWinding);
    //paint.setStroke(true);
    //paint.setStrokeWidth(60);
    //paint.setBlendMode(SkBlendMode::kClear);
    //back->drawPath(path, paint);
    //back->restore();


    HDC hdc = GetDC(hwnd);
    static BITMAPINFO info = {sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0};
    SetDIBits(hdc, bottomHbitmap, 0, h, pixBack->addr(), &info, DIB_RGB_COLORS);
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
    surfaceBack = SkSurfaces::Raster(info);
    pixBack = new SkPixmap();
    surfaceBack->peekPixels(pixBack);
    surfaceFront = SkSurfaces::Raster(info);
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
        case WM_REFRESH: {
            obj->Refresh();
            return true;
        }
        case WM_SETCURSOR:
        {
            return true;
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