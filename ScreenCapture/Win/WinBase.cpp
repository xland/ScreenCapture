#include "pch.h"
#include "Util.h"
#include "WinBase.h"
#include "App.h"

WinBase::WinBase(const int& x, const int& y, const int& w, const int& h) : x{x}, y{y}, w{w}, h{h}
{}

WinBase::~WinBase()
{}

void WinBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
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
	if (render) {
		render->Resize(D2D1::SizeU(w, h));
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
void WinBase::enableAlpha()
{
    HRGN region = CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb = { 0 };
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = region;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(hwnd, &bb);
    DeleteObject(region);
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

LRESULT WinBase::onHitTest(WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
}
void WinBase::createWindow(const DWORD& exStyle, const DWORD& style)
{
    hwnd = CreateWindowEx(exStyle, getWinClsName().c_str(), NULL, style, x, y, w, h, NULL, NULL, App::get()->hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	dpi = GetDpiForWindow(hwnd) / 96.0f;
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, //D2D1_RENDER_TARGET_TYPE_DEFAULT,//D2D1_RENDER_TARGET_TYPE_HARDWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96
    );
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);
    auto d2d = Util::getD2D();
    auto hr = d2d->CreateHwndRenderTarget(rtProps, hwndProps, render.GetAddressOf());
    if (FAILED(hr))  log(L"CreateHwndRenderTarget error");
}

bool WinBase::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_CROSS));
    return TRUE;
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
        return self->paint();
    }
    else if (msg == WM_SETCURSOR) {
        if (LOWORD(lParam) == HTCLIENT) return self->onCursor();
    }
    else if (msg == WM_RBUTTONDOWN)
    {
        self->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), true);
    }
    else if (msg == WM_LBUTTONDOWN) {
        self->isMouseDown = true;
        self->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), false);
    }
    else if (msg == WM_LBUTTONUP) {
        self->isMouseDown = false;
        self->onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    }
    else if (msg == WM_MOUSEMOVE) {
        if (self->isMouseDown) {
            self->onMouseDrag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
    else if (msg == WM_TIMER) {
        self->onTimer(wParam-WM_APP);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WinBase::paint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    render->BeginDraw();
    onPaint();
    render->EndDraw();
    EndPaint(hwnd, &ps);
    return 1;
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

ComPtr<IDWriteTextLayout> WinBase::getIconLayout(const std::wstring& icon, const float& fontSize, const float& w, const float& h)
{
    auto format = Util::getIconFormat();
    auto dwrite = Util::getWriteFactory();
    ComPtr<IDWriteTextLayout> layout;
    dwrite->CreateTextLayout(icon.data(), icon.length(), format, w, h, layout.GetAddressOf());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(fontSize, { 0, static_cast<UINT32>(icon.length()) });
    return layout;
}

ComPtr<IDWriteTextLayout> WinBase::getTextLayout(const std::wstring& str, const float& fontSize, const float& w, const float& h)
{
    auto format = Util::getIconFormat();
    auto dwrite = Util::getWriteFactory();
    ComPtr<IDWriteTextLayout> layout;
    dwrite->CreateTextLayout(str.data(), str.length(), format, w, h, layout.GetAddressOf());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(fontSize, { 0, static_cast<UINT32>(str.length()) });
    return layout;
}