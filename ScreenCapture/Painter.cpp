#include "Painter.h"
#include <Windows.h>
#include "Font.h"
#include "Util.h"

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
    unsigned int dataSize = w * h * 4;
    auto bgPixels = new unsigned char[dataSize];
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
    bgImage->createFromData(w, h, BL_FORMAT_PRGB32, bgPixels, w * 4, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
    });
    boardImage = new BLImage();
    boardImage->createFromData(w, h, BL_FORMAT_PRGB32, boardPixels, w * 4, [](void* impl, void* externalData, void* userData) {
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
    paintCtx->begin(*boardImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *bgImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *canvasImage);
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
    BLRectI rectSrc;
    rectSrc.x = pixelX - 10;
    rectSrc.y = pixelY - 5;
    rectSrc.w = 20;
    rectSrc.h = 10;

    BLRectI rectDst;
    rectDst.x = pixelX + 10;
    rectDst.y = pixelY + 10;
    rectDst.w = 200;
    rectDst.h = 100;

    paintCtx->blitImage(rectDst, *bgImage, rectSrc);
    paintCtx->setStrokeStyle(BLRgba32(0,0,0));
    paintCtx->setStrokeWidth(1);
    paintCtx->strokeRect(BLRectI(rectDst.x,rectDst.y,rectDst.w,rectDst.h*2));

    static int crossSize = 10;
    paintCtx->setStrokeStyle(BLRgba32(60, 80,160, 110));
    paintCtx->setStrokeWidth(crossSize);
    paintCtx->strokeLine(rectDst.x, rectDst.y+ rectDst.h/2, rectDst.x+ rectDst.w/2-crossSize/2, rectDst.y + rectDst.h / 2);
    paintCtx->strokeLine(rectDst.x + rectDst.w / 2 + crossSize / 2, rectDst.y + rectDst.h / 2, rectDst.x + rectDst.w, rectDst.y + rectDst.h / 2);
    paintCtx->strokeLine(rectDst.x+ rectDst.w/2, rectDst.y, rectDst.x + rectDst.w/2, rectDst.y + rectDst.h / 2 - crossSize / 2);
    paintCtx->strokeLine(rectDst.x + rectDst.w / 2, rectDst.y + rectDst.h / 2 + crossSize / 2, rectDst.x + rectDst.w / 2, rectDst.y + rectDst.h);

    paintCtx->setFillStyle(BLRgba32(0, 0, 0,180));
    paintCtx->fillRect(BLRectI(rectDst.x, rectDst.y+ rectDst.h, rectDst.w, rectDst.h));

    auto font = Font::Get()->fontText;
    font->setSize(16);
    paintCtx->setFillStyle(BLRgba32(255, 255, 255));
    auto textX = rectDst.x + 10;
    auto textY = rectDst.y + rectDst.h + 14 + font->metrics().ascent;
    auto utf8 = ConvertToUTF8(L"Î»ÖÃ£º");
    paintCtx->fillUtf8Text(BLPoint(textX, textY), *font, utf8.data());
    utf8 = ConvertToUTF8(L"RGB£º");
    paintCtx->fillUtf8Text(BLPoint(textX+1, textY+28), *font, utf8.data());
    utf8 = ConvertToUTF8(L"¸´ÖÆ£º");
    paintCtx->fillUtf8Text(BLPoint(textX, textY+56), *font, utf8.data());
}