#include "MainWin.h"

void MainWin::initCanvas(char* bgPixels, char* boardPixels)
{
    bgImage = new BLImage();
    bgImage->createFromData(w, h, BL_FORMAT_PRGB32, bgPixels, w * 4, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });


    boardImage = new BLImage();
    boardImage->createFromData(w, h, BL_FORMAT_PRGB32, boardPixels, w * 4, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });

    canvasImage = new BLImage(w, h, BL_FORMAT_PRGB32); 
    prepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    paintCtx = new BLContext();
    paintCtx->begin(*prepareImage);
    paintCtx->clearAll();
    paintCtx->end();
    paintCtx->begin(*canvasImage);
    paintCtx->clearAll();

    auto font = Font::Get()->fontText;
    font->setSize(32);
    paintCtx->setFillStyle(BLRgba32(30,30,30));
    auto utf8 = ConvertToUTF8(std::wstring{L"²âÊÔ²âÊÔ²âÊÔ"});
    paintCtx->fillUtf8Text(BLPoint(100,100), *font, utf8.c_str());

    paintCtx->end();
}


bool MainWin::endDrawing()
{
    if (!isDrawing) {
        return false;
    }
    paintCtx->begin(*canvasImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *prepareImage);
    paintCtx->end();
    paintCtx->begin(*prepareImage);
    paintCtx->clearAll();
    paintCtx->end();
    isDrawing = false;
    state = preState;
    History::SaveLast();
    return true;
}

void MainWin::paintBoard() 
{
    paintCtx->begin(*boardImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *bgImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *canvasImage);
    if (isDrawing) {
        paintCtx->blitImage(BLRect(0, 0, w, h), *prepareImage);
    }  
    paintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    paintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
    paintCtx->fillBoxArray(maskBoxes, 8);
    paintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 255));
    paintCtx->setStrokeWidth(3.8f);
    paintCtx->strokeBox(cutBox);
    drawToolMain();
    paintCtx->end();
}

void  MainWin::drawShape(const POINT& pos)
{
    auto history = History::Get();
    if (history->size() < 1) return;
    auto shape = history->at(history->size() - 1);
    paintCtx->begin(*prepareImage);
    paintCtx->clearAll();
    shape->Draw(paintCtx, pos.x, pos.y, mouseDownPos.x, mouseDownPos.y);
    paintCtx->end();
    InvalidateRect(hwnd, nullptr, false);
}

void MainWin::drawPen(const POINT& pos)
{
    auto history = History::Get();
    if (history->size() < 1) return;
    auto shape = history->at(history->size() - 1);
    paintCtx->begin(*prepareImage);
    shape->Draw(paintCtx, pos.x, pos.y, mouseDownPos.x, mouseDownPos.y);
    paintCtx->end();
    mouseDownPos = pos;
    InvalidateRect(hwnd, nullptr, false);
}

void MainWin::drawEraser(const POINT& pos)
{
    auto history = History::Get();
    if (history->size() < 1) return;
    auto shape = history->at(history->size() - 1);
    paintCtx->begin(*canvasImage);
    shape->Draw(paintCtx, pos.x, pos.y, mouseDownPos.x, mouseDownPos.y);
    paintCtx->end();
    mouseDownPos = pos;
    InvalidateRect(hwnd, nullptr, false);
}

