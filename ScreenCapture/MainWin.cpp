#include "MainWin.h"
#include "PinWin.h"


MainWin::MainWin()
{
    state = State::start;
    initLayer();
    shotScreen();
    enumDesktopWindow();
    InitWindow();
    Show();
}
MainWin::~MainWin()
{

}

void MainWin::initLayer()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    stride = w * 4;
    dataSize = stride * h;

    PaintCtx = new BLContext();
    PrepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*PrepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    CanvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*CanvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();

    pixelData = new unsigned char[dataSize];
    BottomImage = new BLImage();
    BottomImage->createFromData(w, h, BL_FORMAT_PRGB32, pixelData, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });
}

void MainWin::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    auto desktopPixel = new unsigned char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, desktopPixel, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    OriginalImage = new BLImage();
    OriginalImage->createFromData(w, h, BL_FORMAT_PRGB32, desktopPixel, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
    });
}

void MainWin::SaveFile(const std::string& filePath) {
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    BLImage imgSave(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(imgSave);
    PaintCtx->blitImage(BLPoint(0, 0), *OriginalImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0, 0), *CanvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->end();
    imgSave.writeToFile(filePath.c_str());
}

void MainWin::BeforePaint() {
    PaintCtx->begin(*BottomImage);
    PaintCtx->blitImage(BLRect(0, 0, w, h), *OriginalImage);
    if (IsMosaicUsePen) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *MosaicImage);
    }
    else
    {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *CanvasImage);
    }
    if (IsDrawing) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *PrepareImage);
    }
    drawMaskBoxes();
    if (state != State::start && !IsDoubleClick) {
        setToolBoxPos();
        drawToolMain();
    }   
    if (state == State::start) {
        drawPixelInfo();
        PaintCtx->end();
    }
    PaintCtx->setStrokeStyle(BLRgba32(22, 119, 255));
    PaintCtx->setStrokeWidth(cutBoxBorderWidth);
    PaintCtx->strokeBox(cutBox);
    PaintCtx->end();
}

void MainWin::PinWindow() {   
    History::LastShapeDrawEnd();
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    auto img = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*img);
    PaintCtx->clearAll();
    PaintCtx->blitImage(BLPoint(0,0), *OriginalImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0,0), *CanvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->end();
    History::Clear();
    new PinWin(x+ cutBox.x0, y+cutBox.y0, img);
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
}


void MainWin::setToolBoxPos()
{
    toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
    auto heightSpan = toolBoxSpan * 3 + toolBoxHeight * 2;

    if (int(h - cutBox.y1) > heightSpan)
    {
        //屏幕底部还有足够的空间
        //两个高度，为屏幕底边也留一点间隙 
        toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
    }
    else if (int(cutBox.y0) > heightSpan)
    {
        //屏幕顶部还有足够的空间
        if (SelectedToolIndex == -1)
        {
            //尚未确定state，主工具条贴着截图区
            toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;
        }
        else
        {
            //已经确定了state，要为子工具条留出区域
            toolBoxMain.y0 = cutBox.y0 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
        }
    }
    else
    {
        //顶部底部都没有足够的空间
        if (SelectedToolIndex == -1)
        {
            //尚未确定state，主工具条贴着截图区底部上方
            toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
        }
        else
        {
            //尚未确定state，主工具条贴着截图区底部上方，并为子工具条留出空间
            toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
        }
        toolBoxMain.x0 -= toolBoxSpan;
    }
    toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
    toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;
    bool flag = true;
    for (size_t i = 0; i < screens.size(); i++)
    {
        auto x = this->x + toolBoxMain.x0 + toolBoxWidth / 2;
        auto y = this->y + toolBoxMain.y0 + toolBoxHeight / 2;
        if (screens[i].contains(x,y)) {
            flag = false;
            break;
        }
    }
    if (flag) {
        //顶部底部都没有足够的空间
        if (SelectedToolIndex == -1)
        {
            //尚未确定state，主工具条贴着截图区底部上方
            toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
        }
        else
        {
            //尚未确定state，主工具条贴着截图区底部上方，并为子工具条留出空间
            toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
        }
        toolBoxMain.x0 -= toolBoxSpan;
        toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
        toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;
    }
}