#include "pch.h"
#include "Util.h"
#include "WinPix.h"
#include "WinCap.h"

WinPix::WinPix() 
{
}
WinPix::~WinPix() 
{
    //textBrush.Reset();
    //bgBrush.Reset();
    //borderBrush.Reset();
    //crossBrush.Reset();
    //render.Reset();
}

void WinPix::init()
{
    imgH = srcH * scaleNum;
    w = srcW * scaleNum;
    h = (imgH + 112);
    createWindow();
    initRender();
}

void WinPix::createWindow()
{
    WNDCLASSEX wx{};
    wx.lpfnWndProc = &WinPix::winMsg;
    wx.cbSize = sizeof(WNDCLASSEX);
    wx.style = CS_HREDRAW | CS_VREDRAW;
    wx.cbClsExtra = 0;
    wx.cbWndExtra = 0;
    wx.hInstance = GetModuleHandle(NULL);
    wx.hIcon = LoadIcon(wx.hInstance, (LPCTSTR)IDI_WINLOGO);
    wx.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wx.lpszMenuName = nullptr;
    wx.lpszClassName = L"WinPix";
    wx.hIconSm = LoadIcon(wx.hInstance, (LPCTSTR)IDI_WINLOGO);
    auto r = RegisterClassEx(&wx);
    if (r == 0) {
        throw std::runtime_error("reg window class error");
    }
    hwnd = CreateWindowEx(WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW, wx.lpszClassName, NULL, WS_POPUP, x, y, w, h, NULL, NULL, wx.hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));    
    dpi = GetDpiForWindow(hwnd) / 96.0f;
    Util::enableHwndAlpha(hwnd);
}

void WinPix::initRender()
{
    render = Util::createRender(hwnd, w, h);
    float crossWHalf{ 5.f };
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.6f), bgBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), borderBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 1.f, 0.5f), crossBrush.GetAddressOf());
    auto dwriteFactory = Util::getWriteFactory();
    dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        15, L"", textFormat.GetAddressOf());
    crossRect0 = D2D1::RectF(0.f, imgH / 2 - crossWHalf, w / 2 - crossWHalf, imgH / 2 + crossWHalf);
    crossRect1 = D2D1::RectF(w / 2 + crossWHalf, imgH / 2 - crossWHalf, w, imgH / 2 + crossWHalf);
    crossRect2 = D2D1::RectF(w / 2 - crossWHalf, 0.f, w / 2 + crossWHalf, imgH / 2 - crossWHalf);
    crossRect3 = D2D1::RectF(w / 2 - crossWHalf, imgH / 2 + crossWHalf, w / 2 + crossWHalf, imgH);
}

LRESULT WinPix::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<WinPix*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!self) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    if (msg == WM_PAINT) {
        self->onPaint();
    } 
    else if (msg == WM_ERASEBKGND) {
        return 1;
    }
    else if (msg == WM_NCHITTEST) {
        return HTTRANSPARENT;//只能把鼠标消息转发给当前进程的窗口，这已经足够了
    }
    else if (msg == WM_KEYDOWN) {
        Util::moveMouse(wParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WinPix::move(const int& x, const int& y)
{
    cursorX = x;
    cursorY = y;
    auto win = WinCap::get();
    auto span{ 10*dpi };
    this->x = x + span;
    if (this->x + w > win->w) {
        this->x = x - span - w;
    }
    this->y = y + span;
    if (this->y + h > win->h) {
        this->y = y - span - h;
    }
    SetWindowPos(hwnd, nullptr, this->x, this->y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
    InvalidateRect(hwnd, nullptr, false);
    ShowWindow(hwnd, SW_SHOW);
}

COLORREF WinPix::paintImg()
{
    auto win = WinCap::get();
    auto wHalf{ srcW / 2 }, hHalf{ srcH / 2 };
    auto x{ cursorX - wHalf }, y{ cursorY - hHalf }, r{ cursorX + wHalf }, b{ cursorY + hHalf };
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, w, imgH);
    if (x < 0) {
        destRect.left = -x * scaleNum;
        wHalf += x;
        x = 0;
    }
    if (y < 0) {
        destRect.top = -y * scaleNum;
        hHalf += y;
        y = 0;
    }
    if (r > win->w) {
        destRect.right = w - (r - win->w) * scaleNum;
        wHalf += (r - win->w);
        r = win->w;
    }
    if (b > win->h) {
        destRect.bottom = imgH - (b - win->h) * scaleNum;
        hHalf += (b - win->h);
        b = win->h;
    }
    auto cpuImg = win->getImgData(x, y, r, b);
    ComPtr<ID2D1Bitmap> imgSrc;
    auto props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    D2D1_MAPPED_RECT mapped{};
    auto hr = cpuImg->Map(D2D1_MAP_OPTIONS_READ, &mapped);
    hr = render->CreateBitmap(cpuImg->GetPixelSize(), mapped.bits, mapped.pitch, &props, imgSrc.GetAddressOf());
    BYTE* pix = mapped.bits + hHalf * mapped.pitch + wHalf * 4;
    hr = cpuImg->Unmap();
    render->DrawBitmap(imgSrc.Get(), destRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    COLORREF cr = RGB(pix[2],pix[1], pix[0]);;
    return cr;
}

void WinPix::paintText(COLORREF& cr, const int& x, const int& y) {

    auto format = textFormat.Get();
    auto fontSize = format->GetFontSize();
    auto padding = 9;
    BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
    wchar_t hex[8];
    swprintf_s(hex, L"#%02X%02X%02X", r, g, b);
    auto str = std::format(L"HEX (Ctrl+H) : {}", hex);
    D2D1_RECT_F rect = D2D1::RectF(padding, imgH+ padding, w, imgH + padding + fontSize );    
    render->DrawText(str.data(), str.length(), format, rect, textBrush.Get());

    rect.top = rect.bottom+padding;
    rect.bottom = rect.top+fontSize;
    str = std::format(L"RGB (Ctrl+R) : {},{},{}", r, g, b);
    render->DrawText(str.data(), str.length(), format, rect, textBrush.Get());

    double R = r / 255.0, G = g / 255.0, B = b / 255.0;
    double K = 1.0 - (std::max)(R, (std::max)(G, B));
    double C = (K == 1.0) ? 0.0 : (1.0 - R - K) / (1.0 - K);
    double M = (K == 1.0) ? 0.0 : (1.0 - G - K) / (1.0 - K);
    double Y = (K == 1.0) ? 0.0 : (1.0 - B - K) / (1.0 - K);
    int c = static_cast<int>(std::round(C * 100));
    int m = static_cast<int>(std::round(M * 100));
    int y1 = static_cast<int>(std::round(Y * 100));
    int k = static_cast<int>(std::round(K * 100));
    rect.top = rect.bottom + padding;
    rect.bottom = rect.top + fontSize;
    str = std::format(L"CMYK (Ctrl+K) : {},{},{},{}", c, m, y1, k);
    render->DrawText(str.data(), str.length(), format, rect, textBrush.Get());
    rect.top = rect.bottom + padding;
    rect.bottom = rect.top + fontSize;
    str = std::format(L"POS (Ctrl+P) : X:{} Y:{}", cursorX, cursorY);
    render->DrawText(str.data(), str.length(), format, rect, textBrush.Get());
}

void WinPix::onPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    render->BeginDraw();
    render->Clear(D2D1::ColorF(0x000000, 0.3));
    auto r = D2D1::RectF(0, 0, w, h);
    render->FillRectangle(r, bgBrush.Get());
    auto cr = paintImg();
    render->DrawRectangle(r, borderBrush.Get(), 2.f);
    render->FillRectangle(crossRect0, crossBrush.Get());
    render->FillRectangle(crossRect1, crossBrush.Get());
    render->FillRectangle(crossRect2, crossBrush.Get());
    render->FillRectangle(crossRect3, crossBrush.Get());
    paintText(cr, x, y);
    render->EndDraw();
    EndPaint(hwnd, &ps);
}

void WinPix::close()
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    DestroyWindow(hwnd);
}
