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
    if (state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->setStrokeStyle(BLRgba32(22, 119, 255));
    PaintCtx->setStrokeWidth(cutBoxBorderWidth);
    PaintCtx->strokeBox(cutBox);
    if (state != State::start && !IsDoubleClick) {
        setToolBoxPos();
        drawToolMain();
    }
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

int MainWin::getScreenIndex(const double& x, const double& y) {
    int index = -1;
    for (size_t i = 0; i < screens.size(); i++)
    {
        if (screens[i].contains(x, y)) {
            index = i;
            break;
        }
    }
    return index;
}

void MainWin::setToolBoxPos()
{
    //三个缝隙高度和两个工具条高度
    auto heightSpan = toolBoxSpan * 3 + toolBoxHeight * 2;
    //工具条右下角是否在屏幕内(多减2个像素控制边界)
    auto index = getScreenIndex(cutBox.x1-2, cutBox.y1 + heightSpan);
    if (index >= 0) {
        //工具条左上角是否在屏幕内
        if (getScreenIndex(cutBox.x1 - toolBoxWidth, cutBox.y1 + toolBoxSpan)>=0) {
            toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
            toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
        }
        else
        {
            //工具条左上角不在屏幕中
            toolBoxMain.x0 = screens[index].x0;
            toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
        }
    }    
    else
    {
        //判断屏幕顶部是否有足够的空间，工具条右上角是否在屏幕内
        index = getScreenIndex(cutBox.x1, cutBox.y0 - heightSpan);
        if (index >= 0) {
            //工具条左上角是否在屏幕内
            if (getScreenIndex(cutBox.x1 - toolBoxWidth, cutBox.y0 - heightSpan) >= 0) {
                toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
                if (SelectedToolIndex == -1)
                {
                    //不需要显示子工具条，主工具条贴着截图区
                    toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;
                }
                else
                {
                    //需要显示子工具条，要为子工具条留出区域
                    toolBoxMain.y0 = cutBox.y0 - toolBoxSpan * 2 - toolBoxHeight * 2;
                } 
            }
            else
            {
                //工具条左上角不在屏幕中
                toolBoxMain.x0 = screens[index].x0;
                if (SelectedToolIndex == -1)
                {
                    //不需要显示子工具条，主工具条贴着截图区
                    toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;
                }
                else
                {
                    //需要显示子工具条，要为子工具条留出区域
                    toolBoxMain.y0 = cutBox.y0 - toolBoxSpan * 2 - toolBoxHeight * 2;
                }
            }
        }
        else
        {
            //屏幕顶部和屏幕底部都没有足够的空间，工具条显示在截图区域内
            //工具条左上角是否在屏幕内
            if (getScreenIndex(cutBox.x1 - toolBoxWidth, cutBox.y1 - heightSpan) >= 0) {
                toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
                if (SelectedToolIndex == -1)
                {
                    //不需要显示子工具条，主工具条贴着截图区
                    toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
                }
                else
                {
                    //需要显示子工具条，要为子工具条留出区域
                    toolBoxMain.y0 = cutBox.y1 - toolBoxSpan * 2 - toolBoxHeight * 2;
                }
            }
            else
            {
                //工具条左上角不在屏幕中，得到截图区域所在屏幕
                index = getScreenIndex(cutBox.x1, cutBox.y1);
                if (index >= 0) {
                    toolBoxMain.x0 = screens[index].x0;
                    if (SelectedToolIndex == -1)
                    {
                        //不需要显示子工具条，主工具条贴着截图区
                        toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
                    }
                    else
                    {
                        //需要显示子工具条，要为子工具条留出区域
                        toolBoxMain.y0 = cutBox.y1 - toolBoxSpan * 2 - toolBoxHeight * 2;
                    }
                }
            }
        }
    }
    toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
    toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;
}