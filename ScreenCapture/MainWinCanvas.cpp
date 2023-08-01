#include "MainWin.h"



void MainWin::paintBoard() 
{
    painter->paintCtx->begin(*painter->boardImage);
    painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->bgImage);
    painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->canvasImage);
    if (painter->isDrawing) {
        painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->prepareImage);
    }  
    painter->paintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
    painter->paintCtx->fillBoxArray(maskBoxes, 8);
    painter->paintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 255));
    painter->paintCtx->setStrokeWidth(3.8f);
    painter->paintCtx->strokeBox(cutBox);
    drawToolMain();
    painter->paintCtx->end();
}

