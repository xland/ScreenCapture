#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinLong.h"
#include "CutMask.h"
#include "WinStart.h"

std::unique_ptr<WinLong> winLong;

WinLong::WinLong(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{

}

WinLong::~WinLong()
{

}

void WinLong::init()
{
    WinLong::release();
    auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	std::vector<BYTE> data = Util::captureScreen(x, y, w, h);
    winLong = std::make_unique<WinLong>(x, y, w, h);
    winLong->createWindow(WS_EX_TOPMOST);
    D2D1_BITMAP_PROPERTIES1 props = {
        .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
        .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET}
    };
    winLong->render->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, winLong->screenImg.GetAddressOf());
    POINT pt;
    GetCursorPos(&pt);
    winLong->cutMask = std::make_unique<CutMask>(winLong.get());
    winLong->show();
    UpdateWindow(winLong->hwnd);

}
WinLong* WinLong::get()
{
    return winLong.get();
}

void WinLong::release()
{
    if (winLong.get()) {
        winLong->close();
        winLong.reset();
    }
}
ComPtr<ID2D1Bitmap1> WinLong::getCutImg()
{
    auto& r = cutMask->maskRect;
    auto rect = D2D1::RectU((UINT32)r.left, (UINT32)r.top, (UINT32)r.right, (UINT32)r.bottom);
	return getImgByRect(rect);
}
ComPtr<ID2D1Bitmap1> WinLong::getImgByRect(D2D1_RECT_U& rect)
{
    ComPtr<ID2D1Bitmap1> cpuImg;
    ComPtr<ID2D1DeviceContext> dc;
    render->QueryInterface(IID_PPV_ARGS(dc.GetAddressOf()));
    D2D1_BITMAP_PROPERTIES1 prop1{};
    prop1.pixelFormat = screenImg->GetPixelFormat();
    prop1.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    screenImg->GetDpi(&prop1.dpiX, &prop1.dpiY);
    dc->CreateBitmap(D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top), nullptr, 0, &prop1, cpuImg.GetAddressOf());
    auto start = D2D1::Point2U(0, 0);
    auto hr = cpuImg->CopyFromBitmap(&start, screenImg.Get(), &rect);
    return cpuImg;
}

void WinLong::onPaint()
{
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawBitmap(screenImg.Get(), destRect);
    cutMask->paint();
}

void WinLong::onMouseMove(const int& x, const int& y) {
    if (isFinishCutMask)  return;
    cutMask->highlight(x, y);
}
void WinLong::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (isFinishCutMask)  return;
    cutMask->makeRect(x, y);
}
void WinLong::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        App::exit(2);
        return;
    }
    if (isFinishCutMask)  return;
    cutMask->startMakeRect(x, y);
}
void WinLong::onMouseUp(const int& x, const int& y)
{
    isFinishCutMask = true;;
    HRGN rgn1 = CreateRectRgn(0, 0, w, h);
    auto& r = cutMask->maskRect;
    HRGN rgn2 = CreateRectRgn(r.left, r.top, r.right, r.bottom);
    CombineRgn(rgn1, rgn1, rgn2, RGN_DIFF);
    SetWindowRgn(hwnd, rgn1, true);
	winStart = std::make_unique<WinStart>(x, y, 60*dpi, 60 * dpi);
}
