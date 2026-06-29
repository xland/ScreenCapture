#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinCap.h"
#include "WinCutMask.h"
#include "WinPix.h"
#include "WinPin.h"
#include "History.h"

std::unique_ptr<WinCap> winCap;

WinCap::WinCap(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{

}

WinCap::~WinCap()
{

}

void WinCap::init()
{
    WinCap::release();
    auto [x, y, w, h] = Util::getDesktopInfo();
	winCap = std::make_unique<WinCap>(x, y, w, h);
    winCap->createWindow(WS_EX_TOPMOST);
}
WinCap* WinCap::get()
{
    return winCap.get();
}

void WinCap::release(bool allowAutoQuit)
{
    if (winCap.get()) {
        if (winCap->winPix) {
            winCap->winPix->close();
        }
        winCap->close();
        winCap.reset();
        if (allowAutoQuit && App::getArg(L"auto-quit") == L"true") {
            App::exit(0);
        }
    }
}
ComPtr<ID2D1Bitmap1> WinCap::getCutImg()
{
    auto& r = cutMask->maskRect;
    auto rect = D2D1::RectU((UINT32)r.left, (UINT32)r.top, (UINT32)r.right, (UINT32)r.bottom);
	return getImgByRect(rect);
}
ComPtr<ID2D1Bitmap1> WinCap::getImgByRect(D2D1_RECT_U& rect)
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

void WinCap::onCreated()
{
    std::vector<BYTE> data = Util::captureScreen(x, y, w, h);
    D2D1_BITMAP_PROPERTIES1 props = {
        .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
        .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET}
    };
    render->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, winCap->screenImg.GetAddressOf());
    POINT pt;
    GetCursorPos(&pt);
    winPix = std::make_unique<WinPix>((int)pt.x, (int)pt.y);
    cutMask = std::make_unique<WinCutMask>(this);
    show();
}

void WinCap::onPaint()
{
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawBitmap(screenImg.Get(), destRect);
    cutMask->paint();
}

void WinCap::onMouseMove(const int& x, const int& y) {
    cutMask->highlight(x, y);
    winPix->move(x, y);
}
void WinCap::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (!isDraggingRect) {
        auto dx = std::abs(x - pressX);
        auto dy = std::abs(y - pressY);
        if (dx < 3 && dy < 3) {
            return;
        }
        isDraggingRect = true;
        cutMask->startMakeRect(pressX, pressY);
    }
    cutMask->makeRect(x, y);
}
void WinCap::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        WinCap::release();
        return;
    }
    pressX = x;
    pressY = y;
    isDraggingRect = false;
    if (winPix.get()) {
        winPix->hide(); //必须先hide再释放，不然会闪烁一下
        winPix->close();
        winPix.reset();
    }
}
void WinCap::onMouseUp(const int& x, const int& y)
{
    if (isDraggingRect) {
        cutMask->makeRect(x, y);
    }
    else {
        cutMask->highlight(x, y);
    }
    WinPin::init();
    WinCap::release(false);
}

void WinCap::onKeyDown(const UINT& key)
{
    if (key == VK_ESCAPE) {
        WinCap::release();
    }
}
