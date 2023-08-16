#include "Painter.h"
#include <Windows.h>
#include "Font.h"
#include "Util.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <format>

static Painter* p;

void Painter::Init()
{
    p = new Painter();
}
Painter* Painter::Get()
{
    return p;
}
void Painter::Dispose()
{
    delete p;
}
Painter::Painter()
{
    shotScreen();
}
Painter::~Painter()
{
    delete paintCtx;
    delete bgImage;
    delete canvasImage;
    delete prepareImage;
    //delete boardImage; //todo
    DeleteObject(bgHbitmap);
}
void Painter::shotScreen()
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
    unsigned int stride = w * 4;
    unsigned int dataSize = stride * h;
    bgPixels = new unsigned char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, (LPVOID)bgPixels, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);

    auto boardPixels = new unsigned char[dataSize];
    bgHbitmap = CreateDIBSection(hScreen, &info, DIB_RGB_COLORS, reinterpret_cast<void**>(&boardPixels), NULL, NULL);
    ReleaseDC(NULL, hScreen);


    canvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    prepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    bgImage = new BLImage();
    bgImage->createFromData(w, h, BL_FORMAT_PRGB32, bgPixels, stride, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
    });
    boardImage = new BLImage();
    boardImage->createFromData(w, h, BL_FORMAT_PRGB32, boardPixels, stride, [](void* impl, void* externalData, void* userData) {
        delete[] externalData; //todo
    });
    paintCtx = new BLContext();
    paintCtx->begin(*prepareImage);
    paintCtx->clearAll();
    paintCtx->end();
    paintCtx->begin(*canvasImage);
    paintCtx->clearAll();
    paintCtx->end();
}

BLContext* Painter::PaintBoard()
{
    //BLContextCreateInfo createInfo{};
    //createInfo.threadCount = 3;
    //paintCtx->begin(*boardImage,createInfo);
    paintCtx->begin(*boardImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *bgImage);
    if (IsMosaicUsePen) {
        paintCtx->blitImage(BLRect(0, 0, w, h), *mosaicImage);
    }
    else
    {
        paintCtx->blitImage(BLRect(0, 0, w, h), *canvasImage);
    }    
    if (isDrawing) {
        paintCtx->blitImage(BLRect(0, 0, w, h), *prepareImage);
    }
    return paintCtx;
}

void Painter::PaintOnWindow(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HDC hdcBmp = CreateCompatibleDC(hdc);
    DeleteObject(SelectObject(hdcBmp, bgHbitmap));
    BitBlt(hdc, 0, 0, (int)w, (int)h, hdcBmp, 0, 0, SRCCOPY);
    DeleteDC(hdcBmp);
    EndPaint(hwnd, &ps);
    ValidateRect(hwnd, NULL);
}

void Painter::DrawPixelInfo()
{
    if (pixelY == -999999 && pixelX == -999999) {
        return;
    }
    BLRectI rectDst;
    rectDst.x = pixelX + 10;
    rectDst.y = pixelY + 10;
    rectDst.w = 200;
    rectDst.h = 100;
    if (rectDst.x + rectDst.w > w) {
        rectDst.x -= (rectDst.w+20);
    }
    if (rectDst.y + rectDst.w > h) {
        rectDst.y -= (rectDst.w +20);
    }
    {
        auto srcX = pixelX - 10;
        auto srcY = pixelY - 5;
        auto dstX = rectDst.x;
        auto dstY = rectDst.y;    
        static size_t dataSize = w * 4 * h;
        for (size_t y = 0; y < 10; y++)
        {
            for (size_t x = 0; x < 20; x++) {
                static BLRgba32 color;
                if (srcX <0 || srcY <0 || srcX >= w || srcY >= h) {
                    color.setB(0);
                    color.setG(0);
                    color.setR(0);
                    color.setA(255);
                }
                else
                {
                    size_t indexSrc = srcY * w * 4 + srcX * 4;
                    color.setB(bgPixels[indexSrc]);
                    color.setG(bgPixels[indexSrc + 1]);
                    color.setR(bgPixels[indexSrc + 2]);
                    color.setA(255);
                }
                paintCtx->setFillStyle(color);
                paintCtx->fillRect(dstX, dstY, 10, 10);
                dstX += 10;
                srcX += 1;
            }
            dstX = rectDst.x;
            dstY += 10;
            srcX = pixelX - 10;
            srcY += 1;
        }
    }
    paintCtx->setStrokeStyle(BLRgba32(0,0,0));
    paintCtx->setStrokeWidth(1);
    paintCtx->strokeRect(BLRectI(rectDst.x,rectDst.y,rectDst.w,rectDst.h*2));    
    {
        int halfCrossW = 5;
        int halfW = rectDst.w / 2;
        int halfH = rectDst.h / 2;
        paintCtx->setStrokeStyle(BLRgba32(22, 119,255, 110));
        paintCtx->setStrokeWidth(halfCrossW*2);
        paintCtx->strokeLine(rectDst.x, rectDst.y+ halfH, rectDst.x+ halfW - halfCrossW, rectDst.y + halfH);
        paintCtx->strokeLine(rectDst.x + halfW + halfCrossW, rectDst.y + halfH, rectDst.x + rectDst.w, rectDst.y + halfH);
        paintCtx->strokeLine(rectDst.x+ halfW, rectDst.y, rectDst.x + halfW, rectDst.y + halfH - halfCrossW);
        paintCtx->strokeLine(rectDst.x + halfW, rectDst.y + halfH + halfCrossW, rectDst.x + halfW, rectDst.y + rectDst.h);
    }
    paintCtx->setFillStyle(BLRgba32(0, 0, 0,200));
    paintCtx->fillRect(BLRectI(rectDst.x, rectDst.y+ rectDst.h, rectDst.w, rectDst.h));

    auto font = Font::Get()->fontText;
    font->setSize(16.6);
    paintCtx->setFillStyle(BLRgba32(255, 255, 255));
    auto textX = rectDst.x + 8;
    auto textY = rectDst.y + rectDst.h + 14 + font->metrics().ascent;
    std::string utf8 = std::format("Position:X:{} Y:{}", pixelX, pixelY);
    paintCtx->fillUtf8Text(BLPoint(textX, textY), *font, utf8.data()); 
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    utf8 = std::format("RGB(Ctr+R):{},{},{}", bgPixels[pixelIndex + 2], bgPixels[pixelIndex + 1], bgPixels[pixelIndex]);
    paintCtx->fillUtf8Text(BLPoint(textX, textY+28), *font, utf8.data());
    std::stringstream ss;
    ss << std::hex << (bgPixels[pixelIndex + 2] << 16 | bgPixels[pixelIndex + 1] << 8 | bgPixels[pixelIndex]);
    std::string hex = ss.str();
    size_t str_length = hex.length();
    for (size_t i = 0; i < 6 - str_length; i++) {
        hex = "0" + hex;
    }
    utf8 = std::format("HEX(Ctr+H):#{}", hex);
    paintCtx->fillUtf8Text(BLPoint(textX, textY + 56), *font, utf8.data());
}


std::wstring Painter::GetPixelColorRgb()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    auto result = std::to_wstring(bgPixels[pixelIndex + 2]) + L"," +
        std::to_wstring(bgPixels[pixelIndex + 1]) + L"," +
        std::to_wstring(bgPixels[pixelIndex]);
    return result;
}
std::wstring Painter::GetPixelColorHex()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    std::wstringstream ss;
    int rgb = ((bgPixels[pixelIndex + 2] << 16) | (bgPixels[pixelIndex + 1] << 8) | bgPixels[pixelIndex]);
    ss << std::hex << rgb;
    std::wstring hexColorStr = ss.str();
    int str_length = hexColorStr.length();
    for (int i = 0; i < 6 - str_length; i++) {
        hexColorStr = L"0" + hexColorStr;
    }
    return L"#" + hexColorStr;
}