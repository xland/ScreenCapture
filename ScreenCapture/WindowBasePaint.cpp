#include "WindowBase.h"

void WindowBase::InitLayerImg() {
    CanvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PrepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    bottomImage = new BLImage(w, h, BL_FORMAT_PRGB32);

    PaintCtx = new BLContext();
    PaintCtx->begin(*PrepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*CanvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*bottomImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    
    BLImageData imgData;
    bottomImage->getData(&imgData);
    pixelData = (unsigned char*)imgData.pixelData;
    stride = imgData.stride;
}

bool WindowBase::OnPaint()
{
    PaintCtx->begin(*bottomImage);
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
    DrawMaskBox();
    drawToolMain();
    if (!IsLeftBtnDown && state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->end();
    //bottomImage->writeToFile("allen.png");
    return true;
}

void WindowBase::Refresh()
{
    //InvalidateRect(hwnd, nullptr, false);
    //paint();
    auto result = OnPaint();
    painter->Paint(w, h, pixelData, stride);

}