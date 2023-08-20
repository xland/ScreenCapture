#include "WindowBase.h"
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
}

WindowBase::~WindowBase()
{
    delete bgImage;
    delete d2DImage;
    delete render;
    delete factory;
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

void WindowBase::InitWindow(const int& x, const int& y, const unsigned int& w, const unsigned int& h, const bool& shadow)
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
    bgImage = new BLImage(w, h, BL_FORMAT_PRGB32);

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
    hwnd = CreateWindowEx(0, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    //hwnd = CreateWindowEx(0, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    if (shadow) {
        MARGINS m{ 0, 0, 0, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &m);        
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    Show();
}

void WindowBase::d2DCreateRes()
{
    if (render) return;
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0;
    bmpPorp.dpiY = 0;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    BLImageData imgData;
    bgImage->getData(&imgData);
    render->CreateBitmap(size, imgData.pixelData, imgData.stride, bmpPorp, &d2DImage);
}

LRESULT CALLBACK WindowBase::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                return FALSE;
            }
            break;
        }
        case WM_NCHITTEST:
        {
            return HTCAPTION;
        }
        case WM_PAINT:
        {
            d2DCreateRes();
            render->BeginDraw();
            render->DrawBitmap(d2DImage, D2D1::RectF(0, 0, w, h));
            auto result = render->EndDraw();
            if (D2DERR_RECREATE_TARGET == result)
            {
                d2DImage->Release();
                render->Release();
                render = nullptr;
            }
            ValidateRect(hwnd, NULL);
            return TRUE;
        }
    }
    
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}