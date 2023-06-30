#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance},
    direct2dFactory{nullptr},hwnd{nullptr}
{
    shotScreen();
    HRESULT hr;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2dFactory);
    if (!SUCCEEDED(hr)) {
        return;
    }
    createWindow();
}
MainWin::~MainWin()
{
    SafeRelease(&direct2dFactory);
    SafeRelease(&renderTarget);
    SafeRelease(&brush);
}
void MainWin::shotScreen()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int dataSize = w  * h * 4;
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    bgPixels = new char[dataSize];
    int r = GetDIBits(hDC, hBitmap, 0, h, (LPVOID)bgPixels, &Info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}
void MainWin::createWindow()
{
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWin::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = hinstance;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = L"ScreenCapture";
    RegisterClassEx(&wcex);
    hwnd = CreateWindow(wcex.lpszClassName,wcex.lpszClassName,WS_POPUP,x,y,w,h,NULL,NULL,hinstance,this);
    initScaleFactor();
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}

void MainWin::initScaleFactor()
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

LRESULT CALLBACK MainWin::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        MainWin* mainWin = (MainWin*)pcs->lpCreateParams;
        SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(mainWin));
        return 1;
    }
    else
    {
        MainWin* mainWin = reinterpret_cast<MainWin*>(static_cast<LONG_PTR>(GetWindowLongPtr( hwnd, GWLP_USERDATA )));
        if (!mainWin) {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        switch (message)
        {
            case WM_NCCALCSIZE: 
            {
                return 0;
            }
            case WM_PAINT:
            {
                mainWin->Paint();
                ValidateRect(hwnd, NULL);
                return 0;
                break;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 1;
                break;
            }
            default: {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
    }
    return 0;
}

void MainWin::Paint()
{
    createDeviceRes();
    renderTarget->BeginDraw();
    /*renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    renderTarget->DrawLine(D2D1::Point2F(0.0f, 0.0f),D2D1::Point2F(w, h),brush,3.5f);
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(1100.f, 1100.f), 200.f, 350.f);
    renderTarget->DrawEllipse(ellipse, brush, 10.f);*/

    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0.0f;
    bmpPorp.dpiY = 0.0f;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    ID2D1Bitmap* bitmap;
    renderTarget->CreateBitmap(D2D1::SizeU(w, h), (void*)bgPixels, 4 * w, bmpPorp, &bitmap);
    renderTarget->DrawBitmap(bitmap,D2D1::RectF(x,y,w/scaleFactor,h/scaleFactor));


    HRESULT hr = renderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        SafeRelease(&renderTarget);
        SafeRelease(&brush);
    }
}
void MainWin::createDeviceRes()
{
    if (renderTarget) return;
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    HRESULT hr = direct2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &renderTarget);
    if (SUCCEEDED(hr))
    {
        hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),&brush);
    }
}

template<class Interface>
void MainWin::SafeRelease(Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}