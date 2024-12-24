#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "WinBase.h"
#include "../App/Util.h"

WinBase::WinBase(QObject *parent):QObject(parent)
{
}

WinBase::~WinBase()
{
}


void WinBase::initWindow(bool isTransparent)
{
    WNDCLASSEX wcx{};
    auto hinstance = GetModuleHandle(NULL);
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &WinBase::routeWinMsg;
    wcx.cbWndExtra = sizeof(WinBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = L"ScreenCapture";
    auto flag = RegisterClassEx(&wcx);
    auto exStyle = isTransparent ? (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST) : (WS_EX_LAYERED); //WS_EX_TOPMOST
    auto style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    hwnd = CreateWindowEx(exStyle,L"ScreenCapture", L"ScreenCapture",style,x, y, w, h, NULL, NULL, GetModuleHandle(NULL), static_cast<LPVOID>(this));
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));//移除窗口打开与关闭时的动画效果

}
LRESULT WinBase::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<WinBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj ) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE) {
                NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                pncsp->rgrc[0] = pncsp->rgrc[1]; //窗口客户区覆盖整个窗口
                return 0; //确认改变窗口客户区
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }
        case WM_DESTROY:
        {
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            UnregisterClass(L"ScreenCapture", nullptr);
            return 0;
        }
	    default:
	    {
		    return obj->processWinMsg(msg, wParam, lParam);
	    }
    }
}
LRESULT WinBase::processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_MOVE: {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == VK_BACK || wParam == VK_DELETE)
        {
            removeShape();
            return 0;
        }
        else if (wParam == VK_ESCAPE)
        {
            keyEscPress();
            return 0;
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonPress);
        mousePress(&e);
        break;
    }
    case WM_MOUSEMOVE:
    {
        auto e = Util::createMouseEvent(lParam, QEvent::MouseMove);
        if (wParam & MK_LBUTTON) {
            mouseDrag(&e);
        }
        else {
            mouseMove(&e);
        }
        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonDblClick);
        mouseDBClick(&e);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonRelease);
        mouseRelease(&e);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonPress, Qt::MouseButton::RightButton);
        mousePressRight(&e);
        return 0;
    }
    default:
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
    return 0;
}
void WinBase::initSizeByWin(WinBase* win)
{
    x = win->x;
    y = win->y;
    w = win->w;
    h = win->h;
}
QImage WinBase::grab(const QRect& rect)
{
    return img.copy(rect);
}

void WinBase::raise()
{
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
void WinBase::move(const int& x, const int& y)
{
    this->x = x;
    this->y = y;
    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
void WinBase::close()
{
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    deleteLater();
}
void WinBase::releaseImg()
{
    if (!img.isNull()) {
        img = QImage();
    }
}
void WinBase::show()
{
    paint();
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}
void WinBase::initImg()
{
    if (img.isNull()) {
        img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    }
}
void WinBase::paint()
{
    if (img.isNull()) return;
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
