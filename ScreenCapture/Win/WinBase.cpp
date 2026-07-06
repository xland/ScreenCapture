#include "pch.h"
#include "Util.h"
#include "Win/WinBase.h"
#include "App.h"



WinBase::WinBase(const int& x, const int& y, const int& w, const int& h) : x{x}, y{y}, w{w}, h{h}
{

}

WinBase::~WinBase()
{}

void WinBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
    //SetForegroundWindow(hwnd);
    //SetFocus(hwnd);
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
    if (!hwnd || !IsWindow(hwnd)) return;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
    DestroyWindow(hwnd);
    onDestroy();
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
            UINT flags = App::allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
            auto hr = swap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, flags);
            createBitmap();
            refresh();
        }
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

void WinBase::handleDeviceLost()
{
    // D3D 设备丢失：释放本窗口绑定到旧设备的所有资源
    render->SetTarget(nullptr);
    targetBmp.Reset();
    swap.Reset();
    render.Reset();
    compVis.Reset();
    compTgt.Reset();
    compDev.Reset();
    // 重建全局设备（旧设备已丢失，必须强制重建）
    App::recreateDevice();
    // 基于新设备重新建立本窗口的 swap chain / render / composition
    App::makeDC(this);
    createBitmap();
    // 让子类重建内部资源（WinSetting / WinPin 已实现 onDpiChanged 重建 brush / layout）
    onDpiChanged();
    refresh();
}


void WinBase::createWindow(const DWORD& exStyle, const DWORD& style, const std::wstring& title)
{
    hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP| WS_EX_TOOLWINDOW| exStyle, getWinClsName().c_str(), title.data(), WS_POPUP | style, x, y, w, h, NULL, NULL, App::get()->hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
	dpi = GetDpiForWindow(hwnd) / 96.0f;
    App::makeDC(this);
    createBitmap();
    onCreated();
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

BOOL WinBase::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_CROSS));
    return TRUE;
}
std::wstring& WinBase::getWinClsName()
{
    static std::wstring clsName = [] {
        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = &WinBase::winProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = App::get()->hInstance;
        wcex.hIcon = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
        wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"ScreenCapture";
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
        return self->onMouseActivate();
    }
    else if (msg == WM_NCHITTEST)
    {
        return self->onHitTest(wParam, lParam);
    }
    else if (msg == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (msg == WM_SHOWWINDOW) {
        if (wParam) {
            self->onShown();
        }
        else {
            self->onHidden();
        }
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
    else if (msg == WM_RBUTTONDBLCLK)
    {
        self->onMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), true);
        return 0;
    }
    else if (msg == WM_LBUTTONDBLCLK)
    {
        self->onMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false);
        return 0;
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
    else if (msg == WM_KEYUP) {
        self->onKeyUp();
    }
    else if (msg == WM_CHAR) {
        self->onChar(wParam);
    }
    else if (msg == WM_IME_STARTCOMPOSITION) {
        self->onIME();
    }
    else if (msg == WM_TIMER) {
        self->onTimer(wParam-WM_APP);
    }
    else if (msg == WM_KILLFOCUS) {
        self->onBlur();
    }
    else if (msg == WM_DPICHANGED) {
        self->dpiChange(wParam,lParam);
        return 0;
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
    auto hr = render->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        // D3D 设备丢失，需要重建渲染目标；限制重试次数避免无限循环
        if (deviceLostRetries < 3) {
            deviceLostRetries++;
            handleDeviceLost();
        }
        EndPaint(hwnd, &ps);
        return;
    }
    deviceLostRetries = 0; // 渲染成功，重置计数
    UINT presentFlags = 0;
    if (App::allowTearing) presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    hr = swap->Present(0, presentFlags);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        if (deviceLostRetries < 3) {
            deviceLostRetries++;
            handleDeviceLost();
        }
    }
    EndPaint(hwnd, &ps);
}

void WinBase::dpiChange(WPARAM wParam, LPARAM lParam)
{
    dpi = LOWORD(wParam) / 96.f;
    const RECT* prcNewWindow = (RECT*)lParam;
    w = prcNewWindow->right - prcNewWindow->left;
    h = prcNewWindow->bottom - prcNewWindow->top;
    x = prcNewWindow->left;
    y = prcNewWindow->top;
    SetWindowPos(hwnd, NULL,x,y,w,h,SWP_NOZORDER | SWP_NOREDRAW);
    render->SetTarget(nullptr);
    targetBmp.Reset();
    UINT flags = App::allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    auto hr = swap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, flags);
    createBitmap();
    onDpiChanged();
    refresh();
}
