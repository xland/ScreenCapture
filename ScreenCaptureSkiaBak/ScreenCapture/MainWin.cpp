#include "MainWin.h"
#include "spdlog/spdlog.h"
#include <string>
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkImageEncoder.h"
#include "include/codec/SkCodec.h"
#include "Color.h"

MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    initWindowGeomtry();
    shotVirtualScreen();
	createNativeWindow();
	initSurface();
    ShowWindow(hwnd, SW_SHOW);
}
MainWin::~MainWin()
{
	disposeSurfaceResource();
}
void MainWin::initWindowGeomtry()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
void MainWin::paint()
{
    if (!isActive) {
        MessageBox(nullptr, L"not active", L"info", MB_OK);
        return;
    }
	auto surface = getSurface();
	auto canvas = surface->getCanvas();
    canvas->drawImage(bgImg,0,0);
	surface->flushAndSubmit();
	HDC dc = GetDC(hwnd);
	SwapBuffers(dc);
	ReleaseDC(hwnd, dc);
}


void MainWin::shotVirtualScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    if (!result) {
        spdlog::get("logger")->info("DeleteObject error");
    }
    result = BitBlt(hDC, 0, 0, width, height, hScreen, x, y, SRCCOPY);
    if (!result) {
        spdlog::get("logger")->info("BitBlt error");
    }
    unsigned int dataSize = ((width * 32 + 31) / 32) * 4 * height;
    std::vector<std::uint8_t> Pixels;
    Pixels.resize(dataSize);
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(width), static_cast<long>(-height), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    int r = GetDIBits(hDC, hBitmap, 0, height, &Pixels[0], &Info, DIB_RGB_COLORS);
    if (r == 0) {
        spdlog::get("logger")->info("GetDIBits error");
    }
    //bgImg = std::make_shared<SkPixmap>(SkImageInfo::MakeN32Premul(width, height), Pixels.data(), width * 4);
    SkPixmap pixmap(SkImageInfo::MakeN32Premul(width, height), Pixels.data(), width * 4);
    SkBitmap bitmap;
    bitmap.installPixels(pixmap);
    if (bitmap.isImmutable()) {
        spdlog::get("logger")->info("isImmutable shared pixel buffer");
    }
    else
    {
        spdlog::get("logger")->info("not Immutable copy pixel buffer");
    }
    bgImg = bitmap.asImage();

    //auto data = SkEncodePixmap(pixmap, SkEncodedImageFormat::kPNG, 100);
    //SkFILEWStream fileStream("allen.png");
    //fileStream.write(data->data(), data->size());
    //fileStream.flush();
    result = DeleteDC(hDC);
    if (!result) {
        spdlog::get("logger")->info("DeleteDC error");
    }
    ReleaseDC(NULL, hScreen);
    result = DeleteObject(hBitmap);
    if (!result) {
        spdlog::get("logger")->info("DeleteObject(hBitmap)");
    }
}