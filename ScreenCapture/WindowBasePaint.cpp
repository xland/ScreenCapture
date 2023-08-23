#include "WindowBase.h"

void WindowBase::InitLayerImg() {
    CanvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PrepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    BottomImage = new BLImage(w, h, BL_FORMAT_PRGB32);

    PaintCtx = new BLContext();
    PaintCtx->begin(*PrepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*CanvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*BottomImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    
    BLImageData imgData;
    BottomImage->getData(&imgData);
    pixelData = (unsigned char*)imgData.pixelData;
    stride = imgData.stride;
}



void WindowBase::Refresh()
{
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
    BeforeDrawTool();
    drawToolMain();
    if (!IsLeftBtnDown && state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->end();
    painter->Paint(w, h, pixelData, stride);

}