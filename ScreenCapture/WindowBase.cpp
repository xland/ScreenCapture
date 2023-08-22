#include "WindowBase.h"
#include <windowsx.h>
#include <dwmapi.h>
#include "Util.h"

/// <summary>
/// 0 undefined
/// 1 quit by press close btn;
/// 2 quit by press right mouse btn;
/// 3 quit by press esc keyboard
/// 4 quit when copy rgb color
/// 5 quit when copy hex color
/// 6 quit when save to file
/// 7 quit when save to clipboard
/// </summary>
static int mainWinQuitCode = 0;

LRESULT CALLBACK WindowBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj) {
        return obj->WindowProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    Font::Dispose();
    delete PaintCtx;
    delete OriginalImage;
    delete CanvasImage;
    delete PrepareImage;
    delete MosaicImage;
    delete bottomImage;
    delete painter;
}

void WindowBase::quitApp(const int& exitCode)
{
    mainWinQuitCode = exitCode;
    CloseWindow(hwnd);
    PostQuitMessage(0);
}
int WindowBase::GetQuitCode()
{
    return mainWinQuitCode;
}

void WindowBase::InitWindow(const bool& shadow)
{
    auto hinstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowBase::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(WindowBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = shadow?L"SCsub":L"SCmain";
    if (!RegisterClassEx(&wcx))
    {
        MessageBox(NULL, L"注册窗口类失败", L"系统提示", NULL);
        return;
    }
    this->hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));//移除窗口打开与关闭时的动画效果
    if (shadow) {
        MARGINS m{ 0, 0, 0, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &m);        
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    initScaleFactor();
    painter = new Painter(hwnd);
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ChangeCursor(IDC_ARROW);
}

void WindowBase::Close()
{
    CloseWindow(hwnd);
}

bool WindowBase::OnTimer(const unsigned int& id)
{
    if (id == 999) {
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (shape) {
            shape->Draw(-1, -1, -1, -1);
        }
    }
    else if (id == 998) {
        saveClipboard();
    }
    return true;
}

void WindowBase::initScaleFactor()
{
    int dpi = GetDpiForWindow(hwnd);
    switch (dpi) {
    case 96:
        scaleFactor = 1.0;
        break;
    case 120:
        scaleFactor = 1.25;
        break;
    case 144:
        scaleFactor = 1.5;
        break;
    case 168:
        scaleFactor = 1.75;
        break;
    case 192:
        scaleFactor = 2.0;
        break;
    case 216:
        scaleFactor = 2.25;
        break;
    case 240:
        scaleFactor = 2.5;
        break;
    case 288:
        scaleFactor = 3.0;
        break;
    case 336:
        scaleFactor = 3.5;
        break;
    default:
        scaleFactor = 1.0;
        break;
    }    
}

LRESULT CALLBACK WindowBase::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        case WM_NCHITTEST:
        {
            return OnHitTest(); //HTCAPTION;
        }
        case WM_SETCURSOR: {
            return true;
        }
        case WM_ERASEBKGND: {
            return true;
        }
        //case WM_PAINT: {
        //    return paint();            
        //}
        case WM_TIMER: {
            return OnTimer(wParam);            
        }
        case WM_RBUTTONDOWN: {
            return OnRightButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_LBUTTONDOWN: {
            return OnLeftButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_LBUTTONUP: {
            return OnLeftButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        case WM_MOUSEMOVE: {
            return OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_KEYDOWN: {
            return OnKeyDown(wParam);
        }
        case WM_KEYUP: {
            return OnKeyUp(wParam);
        }
        case WM_CHAR: {
            return OnChar(wParam);
        }
        case WM_SIZE:
        {
            if (painter) {
                painter->OnResize(hWnd, LOWORD(lParam), HIWORD(lParam));
            }            
            return false;
        }
    }    
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}