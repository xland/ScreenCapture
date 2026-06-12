#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinVideo.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "History.h"
#include "gifski.h"
#include "WinVideoMp4.hpp"

std::unique_ptr<WinVideo> winVideo;

WinVideo::WinVideo(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{

}

WinVideo::~WinVideo()
{

}

void WinVideo::init()
{
    WinVideo::release();
    auto [x, y, w, h] = Util::getDesktopInfo();
	std::vector<BYTE> data = Util::captureScreen(x, y, w, h);
    winVideo = std::make_unique<WinVideo>(x, y, w, h);
    winVideo->createWindow();
    D2D1_BITMAP_PROPERTIES1 props = {
        .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
        .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET}
    };
    winVideo->render->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, winVideo->screenImg.GetAddressOf());
    POINT pt;
    GetCursorPos(&pt);
    winVideo->cutMask = std::make_unique<WinCutMask>(winVideo.get());
    winVideo->show();
    UpdateWindow(winVideo->hwnd);

}

void WinVideo::release()
{
    if (winVideo.get()) {
        winVideo->close();
        winVideo.reset();
    }
}
ComPtr<ID2D1Bitmap1> WinVideo::getCutImg()
{
    auto& r = cutMask->maskRect;
    auto rect = D2D1::RectU((UINT32)r.left, (UINT32)r.top, (UINT32)r.right, (UINT32)r.bottom);
	return getImgByRect(rect);
}
ComPtr<ID2D1Bitmap1> WinVideo::getImgByRect(D2D1_RECT_U& rect)
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

void WinVideo::onPaint()
{
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    render->DrawBitmap(screenImg.Get(), destRect);
    cutMask->paint();
}

void WinVideo::onMouseMove(const int& x, const int& y) {
    cutMask->highlight(x, y);
}
void WinVideo::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    cutMask->makeRect(x, y);
}
void WinVideo::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        App::exit(2);
        return;
    }
    cutMask->startMakeRect(x, y);
}
void WinVideo::onMouseUp(const int& x, const int& y)
{
    WinPin::init();
    WinVideo::release();
}
