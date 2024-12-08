#include <Windows.h>
#include <windowsx.h>
#include "WinBase.h"

WinBase::WinBase(QObject *parent):QObject(parent)
{
}

WinBase::~WinBase()
{
}


void WinBase::initWindow()
{
    static WNDCLASSEX wcx{};
    if (!wcx.lpfnWndProc) {
        auto hinstance = GetModuleHandle(NULL);
        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcx.lpfnWndProc = &WinBase::RouteWinMessage;
        wcx.cbWndExtra = sizeof(WinBase*);
        wcx.hInstance = hinstance;
        wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
        wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
        wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcx.lpszClassName = L"ScreenCapture";
    }
    RegisterClassEx(&wcx);
    auto exStyle = WS_EX_LAYERED;  //| WS_EX_TOPMOST | WS_EX_TOOLWINDOW
    auto style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    hwnd = CreateWindowEx(exStyle,L"ScreenCapture", L"ScreenCapture",style,x, y, w, h, NULL, NULL, GetModuleHandle(NULL), static_cast<LPVOID>(this));
}
LRESULT WinBase::RouteWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WinBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if(!obj) return DefWindowProc(hWnd, msg, wParam, lParam);
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
        case WM_LBUTTONDOWN:
        {
            auto e = createMouseEvent(lParam);
            obj->mousePress(&e);
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            auto e = createMouseEvent(lParam);
            obj->mouseDBClick(&e);
            break;
        }
        case WM_LBUTTONUP:
        {
            auto e = createMouseEvent(lParam);
            obj->mouseRelease(&e);
            break;
        }
        case WM_MOUSEMOVE:
        {
            auto e = createMouseEvent(lParam);
            if (wParam & MK_LBUTTON) {
                obj->mouseDrag(&e);
            }
            else {
                obj->mouseMove(&e);
            }
            break;
        }
        case WM_RBUTTONDOWN:
        {
            auto e = createMouseEvent(lParam);
            obj->mousePressRight(&e);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
QMouseEvent WinBase::createMouseEvent(LPARAM& lParam)
{
    auto x = GET_X_LPARAM(lParam);
    auto y = GET_Y_LPARAM(lParam);
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    return QMouseEvent(QEvent::MouseButtonPress, QPoint(x, y), QPoint(x, y),Qt::LeftButton, Qt::LeftButton, mf);
}


void WinBase::show()
{
    paint();
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

void WinBase::paint()
{
    HDC hdc = GetDC(hwnd);
    auto compDC = CreateCompatibleDC(NULL);
    auto bitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(compDC, bitmap));

    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0 };
    SetDIBits(hdc, bitmap, 0, h, img.bits(), &info, DIB_RGB_COLORS);
    BLENDFUNCTION blend = { .BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA} };
    POINT pSrc = { 0, 0 };
    SIZE sizeWnd = { w, h };
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, compDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);

    DeleteDC(compDC);
    DeleteObject(bitmap);
}
