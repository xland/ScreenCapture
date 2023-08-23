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
    InvalidateRect(hwnd, nullptr, false);
}