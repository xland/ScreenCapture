#include "Painter.h"

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