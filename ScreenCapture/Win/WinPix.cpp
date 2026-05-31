#include "pch.h"
#include "Util.h"
#include "WinPix.h"
#include "WinCap.h"

namespace {
    static constexpr int scaleNum{ 5 }, imgH{5*30}, srcW{ 50 }, srcH{ 30 };
}

WinPix::WinPix(const int& x, const int& y) : WinBase(x, y, srcW* scaleNum, imgH + 112)
{
    createWindow();
    enableAlpha();
    initRes();
    show();
}
WinPix::~WinPix() 
{
    //textBrush.Reset();
    //bgBrush.Reset();
    //borderBrush.Reset();
    //crossBrush.Reset();
    //render.Reset();
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
    WinBase::move(this->x, this->y);
    refresh();
}

void WinPix::initRes()
{
    float crossWHalf{ 5.f };
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.6f), bgBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), borderBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 1.f, 0.5f), crossBrush.GetAddressOf());
    crossRect0 = D2D1::RectF(0.f, imgH / 2 - crossWHalf, w / 2 - crossWHalf, imgH / 2 + crossWHalf);
    crossRect1 = D2D1::RectF(w / 2 + crossWHalf, imgH / 2 - crossWHalf, w, imgH / 2 + crossWHalf);
    crossRect2 = D2D1::RectF(w / 2 - crossWHalf, 0.f, w / 2 + crossWHalf, imgH / 2 - crossWHalf);
    crossRect3 = D2D1::RectF(w / 2 - crossWHalf, imgH / 2 + crossWHalf, w / 2 + crossWHalf, imgH);
}

COLORREF WinPix::paintImg()
{
    auto win = WinCap::get();
    auto wHalf{ srcW / 2 }, hHalf{ srcH / 2 };
    auto x{ cursorX - wHalf }, y{ cursorY - hHalf }, r{ cursorX + wHalf }, b{ cursorY + hHalf };
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, w, imgH);
    if (x < 0) {
        destRect.left = (float)(-x * scaleNum);
        wHalf += x;
        x = 0;
    }
    if (y < 0) {
        destRect.top = (float)(-y * scaleNum);
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
	auto rect = D2D1::RectU(x, y, r, b);
    auto cpuImg = win->getImgByRect(rect);
    ComPtr<ID2D1Bitmap> imgSrc;
    auto props = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
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

    auto fontSize{ 15.0 };
    auto padding{ 9.0 };
    auto format = Util::getTextFormat(fontSize);
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
}

LRESULT WinPix::onHitTest(WPARAM wParam, LPARAM lParam)
{
    return HTTRANSPARENT;
}
