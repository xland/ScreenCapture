#include "MainWin.h"
#include "PinWin.h"

static MainWin* mainWin;


MainWin::MainWin()
{
    initWindowSize();
    shotScreen();
    enumDesktopWindow();    
    InitLayerImg();
    InitWindow();
    Show();
}
MainWin::~MainWin()
{

}
void MainWin::Init()
{
    mainWin = new MainWin();
}
MainWin* MainWin::Get()
{
    return mainWin;
}
void MainWin::Dispose()
{
    delete mainWin;
}

void MainWin::initWindowSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void MainWin::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int stride = w * 4;
    unsigned int dataSize = stride * h;
    auto desktopPixelData = new unsigned char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, desktopPixelData, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);

    OriginalImage = new BLImage();
    OriginalImage->createFromData(w, h, BL_FORMAT_PRGB32, desktopPixelData, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });
}

void MainWin::PinWindow() {
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    auto img = new BLImage (w + 32, h + 32, BL_FORMAT_PRGB32);
    PaintCtx->begin(*img);
    PaintCtx->clearAll();
    PaintCtx->setFillStyle(BLRgba32(100, 100, 100, 100));
    PaintCtx->fillBox(0, 0, 10, 10);
    //PaintCtx->blitImage(BLPoint(16,16), *OriginalImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    //PaintCtx->blitImage(BLPoint(16,16), *CanvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    //BLGradient linear(BLLinearGradientValues(0, 0, 16, 16));
    //linear.addStop(0.0, BLRgba32(0x00000000));
    //linear.addStop(0.5, BLRgba32(0x11000000));
    //linear.addStop(1.0, BLRgba32(0x33000000));
    //PaintCtx->setFillStyle(linear);
    //PaintCtx->fillBox(0, 0, 16, 16);
    //linear.setValues(BLLinearGradientValues(0, 0, 0, 16));
    //PaintCtx->setFillStyle(linear);
    //PaintCtx->fillBox(16, 0, w+16, 16);
    //linear.setValues(BLLinearGradientValues(0, 16, 16, 0));
    //PaintCtx->setFillStyle(linear);
    //PaintCtx->fillBox(w+16, 0, w + 32, 16);
    //PaintCtx->fillBox(16, 0, w+16, 16);
    //PaintCtx->fillBox(, 0, w + 16, 16);
    //PaintCtx->fillBox(0, 16, 16, h+16);
    PaintCtx->end();
    new PinWin(cutBox.x0,cutBox.y0,img);
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
}