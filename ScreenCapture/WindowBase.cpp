#include "WindowBase.h"
#include <windowsx.h>
#include <dwmapi.h>
#include "Util.h"

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
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);//todo error handle
}

WindowBase::~WindowBase()
{
    d2DImage->Release();
    render->Release();
    factory->Release();
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
    wcx.lpszClassName = L"ScreenCapture";
    if (!RegisterClassEx(&wcx))
    {
        MessageBox(NULL, L"注册窗口类失败", L"系统提示", NULL);
        return;
    }
    this->hwnd = CreateWindowEx(0, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));//移除窗口打开与关闭时的动画效果
    if (shadow) {
        MARGINS m{ 0, 0, 0, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &m);        
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    initScaleFactor();
    Show();
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ChangeCursor(IDC_ARROW);
}

void WindowBase::Close()
{

}

void WindowBase::Refresh()
{
    InvalidateRect(hwnd, nullptr, false);
}

bool WindowBase::paint()
{
    auto subResult = OnPaint();
    if (!render){
        D2D1_SIZE_U size = D2D1::SizeU(w, h);
        factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
        render->SetTransform(D2D1::Matrix3x2F::Identity());
        D2D1_BITMAP_PROPERTIES bmpPorp;
        bmpPorp.dpiX = 0;
        bmpPorp.dpiY = 0;
        bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        render->CreateBitmap(size, NULL, NULL, bmpPorp, &d2DImage);
    }
    static auto rect = D2D1::RectU(0, 0, w, h);
    static unsigned int stride = w * 4;
    static unsigned int tempW = w / scaleFactor;
    static unsigned int tempH = h / scaleFactor;
    static auto rect2 = D2D1::RectF(0, 0, tempW, tempH);
    d2DImage->CopyFromMemory(&rect, pixelData, stride);
    render->BeginDraw();
    render->DrawBitmap(d2DImage, rect2);
    auto result = render->EndDraw();
    if (D2DERR_RECREATE_TARGET == result)
    {
        d2DImage->Release();
        render->Release();
        render = nullptr;
    }
    ValidateRect(hwnd, NULL);
    return subResult;
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
        case WM_PAINT: {
            return paint();            
        }
        case WM_TIMER: {
            return OnTimer(wParam);            
        }
        case WM_RBUTTONDOWN: {
            return OnRightButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_RBUTTONUP: {
            return OnRightButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
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
    }    
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}