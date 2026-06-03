#include "pch.h"
#include "Util.h"
#include "Win/WinBase.h"
#include "App.h"

namespace {
    static ComPtr<ID2D1Factory1> d2dFactory;
    static ComPtr<ID3D11Device> d3d;
    static ComPtr<ID2D1Device1> d2dDev;
    static ComPtr<IDXGIFactory5> fac5;
    static ComPtr<IDWriteFactory5> dwriteFactory;
    //static ComPtr<IDWriteRenderingParams> renderingParams;
    static BOOL allowTearing = FALSE; //是否允许撕裂呈现（允许的话效果稍弱，但渲染更快，咱这个应用尽可能的允许）
}

WinBase::WinBase(const int& x, const int& y, const int& w, const int& h) : x{x}, y{y}, w{w}, h{h}
{

}

WinBase::~WinBase()
{}

void WinBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
    //UpdateWindow(hwnd);
}
void WinBase::hide()
{
	ShowWindow(hwnd, SW_HIDE);
}
void WinBase::refresh()
{
    InvalidateRect(hwnd, nullptr, false);
}
void WinBase::close()
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
    DestroyWindow(hwnd);
}
void WinBase::move(const int& x, const int& y)
{
	this->x = x;
	this->y = y;
	SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER| SWP_NOREDRAW);
}

void WinBase::resize(const int& w, const int& h)
{
	this->w = w;
	this->h = h;
	SetWindowPos(hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER| SWP_NOREDRAW);
    if (render.Get()) {
        if (w > 0 && h > 0)
        {
            render->SetTarget(nullptr);
            targetBmp.Reset();
            UINT flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
            auto hr = swap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, flags);
            createBitmap();
        }
        InvalidateRect(hwnd, nullptr, false);
    }
}

void WinBase::enableShadow()
{
    MARGINS margins = { 1,1,1,1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    int value = 2;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
}
void WinBase::setTimer(const UINT& elapse, const UINT& id)
{
    SetTimer(hwnd, WM_APP + id, elapse, nullptr);
}

void WinBase::killTimer(const UINT& id)
{
    KillTimer(hwnd, WM_APP + id);
}

void WinBase::setCursor(LPCWSTR cursorName)
{
    SetCursor(LoadCursor(NULL, cursorName));
}
void WinBase::createWindow(const DWORD& exStyle, const DWORD& style)
{
    hwnd = CreateWindowEx(exStyle | WS_EX_NOREDIRECTIONBITMAP| WS_EX_TOOLWINDOW, getWinClsName().c_str(), NULL, style| WS_POPUP, x, y, w, h, NULL, NULL, App::get()->hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	dpi = GetDpiForWindow(hwnd) / 96.0f;
    initDevice();
    initDC();
    createBitmap();
}

void WinBase::initDevice()
{
    static std::once_flag deviceInitFlag;
    std::call_once(deviceInitFlag, []() {
        D2D1_FACTORY_OPTIONS opt{};
#ifdef DEBUG
        opt.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, opt, d2dFactory.GetAddressOf());
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED,
            nullptr, 0, D3D11_SDK_VERSION, d3d.GetAddressOf(), nullptr, nullptr);
        ComPtr<IDXGIDevice1> dxgiDev;
        d3d.As(&dxgiDev);
        ComPtr<ID2D1Device> d2d;
        hr = d2dFactory->CreateDevice(dxgiDev.Get(), d2d.GetAddressOf());
        d2d.As(&d2dDev);
        hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&fac5));
        hr = fac5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));//判断设备是否允许撕裂呈现
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<::IUnknown**>(dwriteFactory.GetAddressOf()));
        //dwriteFactory->CreateCustomRenderingParams( 1.0f, 0.0f, 1.0f, DWRITE_PIXEL_GEOMETRY_RGB, DWRITE_RENDERING_MODE_GDI_NATURAL, renderingParams.GetAddressOf());
    });
}

HRESULT WinBase::initDC()
{
    ComPtr<IDXGIDevice1> dxgiDev;
    d3d.As(&dxgiDev);
    auto hr = d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, render.GetAddressOf());
    if (FAILED(hr)) return S_FALSE;
    //render->SetTextRenderingParams(renderingParams.Get());
    DXGI_SWAP_CHAIN_DESC1 scd{};
    scd.Width = w;
    scd.Height = h;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.SampleDesc.Count = 1;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    if (allowTearing) scd.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    hr = fac5->CreateSwapChainForComposition(d3d.Get(), &scd, nullptr, swap.GetAddressOf());
    if (FAILED(hr)) return S_FALSE;
    hr = DCompositionCreateDevice(dxgiDev.Get(), IID_PPV_ARGS(compDev.GetAddressOf()));
    if (FAILED(hr)) return S_FALSE;
    hr = compDev->CreateTargetForHwnd(hwnd, TRUE, compTgt.GetAddressOf());
    if (FAILED(hr)) return S_FALSE;
    hr = compDev->CreateVisual(compVis.GetAddressOf());
    if (FAILED(hr)) return S_FALSE;
    hr = compVis->SetContent(swap.Get());
    if (FAILED(hr)) return S_FALSE;
    hr = compTgt->SetRoot(compVis.Get());
    if (FAILED(hr)) return S_FALSE;
    hr = compDev->Commit();
    if (FAILED(hr)) return S_FALSE;
    return S_OK;
}

HRESULT WinBase::createBitmap()
{
    ComPtr<IDXGISurface> surf;
    auto hr = swap->GetBuffer(0, IID_PPV_ARGS(&surf));
    if (FAILED(hr)) return S_FALSE;
    D2D1_BITMAP_PROPERTIES1 bp{};
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    hr = render->CreateBitmapFromDxgiSurface(surf.Get(), &bp, targetBmp.GetAddressOf());
    if (FAILED(hr)) return S_FALSE;
    render->SetTarget(targetBmp.Get());
    // auto dpi{ this->dpi * 96.0f };
    // render->SetDpi(dpi, dpi);
    return S_OK;
}

LRESULT WinBase::onHitTest(WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
}
bool WinBase::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_CROSS));
    return TRUE;
}

ID2D1Factory1* WinBase::getD2D()
{
    return d2dFactory.Get();
}

IDWriteFactory5* WinBase::getWriteFactory()
{
    return dwriteFactory.Get();
}




std::wstring& WinBase::getWinClsName()
{
    static std::wstring clsName = [] {
        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &WinBase::winProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = App::get()->hInstance;
        wcex.hIcon = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
        wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"HorseJs";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
        auto r = RegisterClassEx(&wcex);
        if (r == 0) {
            log(L"err:: reg window class error");
        }
        return wcex.lpszClassName;
        }();
    return clsName;
}

LRESULT WinBase::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<WinBase*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!self) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    else if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    else if (msg == WM_NCHITTEST)
    {
        return self->onHitTest(wParam, lParam);
    }
    else if (msg == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (msg == WM_PAINT)
    {
        self->paint();
        return 1;
    }
    else if (msg == WM_SETCURSOR) {
        if (LOWORD(lParam) == HTCLIENT) return self->onCursor();
    }
    else if (msg == WM_RBUTTONDOWN)
    {
        self->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), true);
    }
    else if (msg == WM_LBUTTONDOWN) {
        SetCapture(hwnd);
        self->isMouseDown = true;
        self->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false);
    }
    else if (msg == WM_LBUTTONUP) {
        ReleaseCapture();
        self->isMouseDown = false;
        self->onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    }
    else if (msg == WM_MOUSEMOVE) {
        if (self->isMouseDown) {
            self->onMouseDrag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
        }
        else {
            self->mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
    }
    else if (msg == WM_MOUSELEAVE) {
        self->mouseLeave();
        return 0;
    }
    else if (msg == WM_MOUSEWHEEL) {
        POINT pt{ (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);
        self->onMouseWheel(pt.x, pt.y, (short)HIWORD(wParam));
        return 0;
    }
    else if (msg == WM_KEYDOWN) {
        self->onKeyDown(wParam);
    }
    else if (msg == WM_IME_STARTCOMPOSITION) {
        self->onIME();
    }
    else if (msg == WM_TIMER) {
        self->onTimer(wParam-WM_APP);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WinBase::mouseMove(const int& x, const int& y)
{
    if (!isMouseIn)
    {
        isMouseIn = true;
        TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);
    }
	onMouseMove(x, y);
}

void WinBase::mouseLeave()
{
	isMouseIn = false;
    TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
    tme.dwFlags = TME_CANCEL | TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
	onMouseLeave();
}

void WinBase::paint()
{
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    render->BeginDraw();
    onPaint();
    render->EndDraw();
    UINT presentFlags = 0;
    if (allowTearing) presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    swap->Present(0, presentFlags);
    EndPaint(hwnd, &ps);
}