#include "MainWin.h"


bool MainWin::endDrawing()
{
    if (!isDrawing) {
        return false;
    }
    if (state == State::text) {
        KillTimer(hwnd, 999);
        auto history = History::Get();
        if (history->size() < 1) return 0;
        auto shape = (Shape::Text*)history->at(history->size() - 1);
        shape->isEnding = true;
        drawShape(mouseDownPos);
    }
    painter->paintCtx->begin(*painter->canvasImage);
    painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->prepareImage);
    painter->paintCtx->end();
    painter->paintCtx->begin(*painter->prepareImage);
    painter->paintCtx->clearAll();
    painter->paintCtx->end();
    isDrawing = false;
    state = preState;
    History::SaveLast();
    return true;
}

void MainWin::paintBoard() 
{
    painter->paintCtx->begin(*painter->boardImage);
    painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->bgImage);
    painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->canvasImage);
    if (isDrawing) {
        painter->paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->prepareImage);
    }  
    painter->paintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    painter->paintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
    painter->paintCtx->fillBoxArray(maskBoxes, 8);
    painter->paintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 255));
    painter->paintCtx->setStrokeWidth(3.8f);
    painter->paintCtx->strokeBox(cutBox);
    drawToolMain();
    painter->paintCtx->end();
}

void  MainWin::drawShape(const POINT& pos)
{
    auto history = History::Get();
    if (history->size() < 1) return;
    auto shape = history->at(history->size() - 1);
    shape->Draw(pos.x, pos.y, mouseDownPos.x, mouseDownPos.y);    
}

