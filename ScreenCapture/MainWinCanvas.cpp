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
    paintCtx = new BLContext();
    paintCtx->begin(*canvasImage);
    paintCtx->clearAll();
    BLPath path;
    path.moveTo(0, 0);
    path.lineTo(w, h);
    path.moveTo(w, 0);
    path.lineTo(0, h);
    paintCtx->setStrokeStyle(BLRgba32(0, 0, 255));
    paintCtx->setStrokeWidth(12.6);
    paintCtx->strokePath(path);


    paintCtx->setCompOp(BL_COMP_OP_CLEAR);
    BLPath path1;
    path1.moveTo(660, 0);
    path1.lineTo(660, h);
    paintCtx->setStrokeStyle(BLRgba32(0, 0, 0));
    paintCtx->setStrokeWidth(16.6);
    paintCtx->strokePath(path1);
    paintCtx->end();
}

void MainWin::paintBoard() 
{
    paintCtx->begin(*boardImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *bgImage);
    paintCtx->blitImage(BLRect(0, 0, w, h), *canvasImage);

    paintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    paintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
    paintCtx->fillBoxArray(maskBoxes, 8);

    paintCtx->end();
}

void MainWin::drawPen(const POINT& pos)
{
    paintCtx->begin(*canvasImage);
    paintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    paintCtx->setStrokeStyle(BLRgba32(123, 33, 0));
    paintCtx->setStrokeWidth(16.6);
    paintCtx->setStrokeCaps(BL_STROKE_CAP_ROUND);
    paintCtx->strokeLine(mouseDownPos.x, mouseDownPos.y, pos.x, pos.y);
    paintCtx->end();
    mouseDownPos = pos;
}

void MainWin::drawEraser(const POINT& pos)
{
    paintCtx->begin(*canvasImage);
    paintCtx->setCompOp(BL_COMP_OP_CLEAR);
    paintCtx->setStrokeStyle(BLRgba32(123, 33, 0));
    paintCtx->setStrokeWidth(16.6);
    paintCtx->setStrokeCaps(BL_STROKE_CAP_ROUND);
    paintCtx->strokeLine(mouseDownPos.x, mouseDownPos.y, pos.x, pos.y);
    paintCtx->end();
    mouseDownPos = pos;
}

void MainWin::drawMask(const POINT& pos)
{
    if (pos.x < mouseDownPos.x)
    {
        maskBoxes[0].x0 = 0;
        maskBoxes[0].x1 = pos.x;
        maskBoxes[1].x0 = pos.x;
        maskBoxes[1].x1 = mouseDownPos.x;
        maskBoxes[2].x0 = mouseDownPos.x;
        maskBoxes[2].x1 = w;
        maskBoxes[3].x0 = mouseDownPos.x;
        maskBoxes[3].x1 = w;
        maskBoxes[4].x0 = mouseDownPos.x;
        maskBoxes[4].x1 = w;
        maskBoxes[5].x0 = pos.x;
        maskBoxes[5].x1 = mouseDownPos.x;
        maskBoxes[6].x0 = 0;
        maskBoxes[6].x1 = pos.x;
        maskBoxes[7].x0 = 0;
        maskBoxes[7].x1 = pos.x;
    }
    else
    {
        maskBoxes[0].x0 = 0;
        maskBoxes[0].x1 = mouseDownPos.x;
        maskBoxes[1].x0 = mouseDownPos.x;
        maskBoxes[1].x1 = pos.x;
        maskBoxes[2].x0 = pos.x;
        maskBoxes[2].x1 = w;
        maskBoxes[3].x0 = pos.x;
        maskBoxes[3].x1 = w;
        maskBoxes[4].x0 = pos.x;
        maskBoxes[4].x1 = w;
        maskBoxes[5].x0 = mouseDownPos.x;
        maskBoxes[5].x1 = pos.x;
        maskBoxes[6].x0 = 0;
        maskBoxes[6].x1 = mouseDownPos.x;
        maskBoxes[7].x0 = 0;
        maskBoxes[7].x1 = mouseDownPos.x;
    }
    if (pos.y < mouseDownPos.y)
    {
        maskBoxes[0].y0 = 0;
        maskBoxes[0].y1 = pos.y;
        maskBoxes[1].y0 = 0;
        maskBoxes[1].y1 = pos.y;
        maskBoxes[2].y0 = 0;
        maskBoxes[2].y1 = pos.y;
        maskBoxes[3].y0 = pos.y;
        maskBoxes[3].y1 = mouseDownPos.y;
        maskBoxes[4].y0 = mouseDownPos.y;
        maskBoxes[4].y1 = h;
        maskBoxes[5].y0 = mouseDownPos.y;
        maskBoxes[5].y1 = h;
        maskBoxes[6].y0 = mouseDownPos.y;
        maskBoxes[6].y1 = h;
        maskBoxes[7].y0 = pos.y;
        maskBoxes[7].y1 = mouseDownPos.y;
    }
    else
    {
        maskBoxes[0].y0 = 0;
        maskBoxes[0].y1 = mouseDownPos.y;
        maskBoxes[1].y0 = 0;
        maskBoxes[1].y1 = mouseDownPos.y;
        maskBoxes[2].y0 = 0;
        maskBoxes[2].y1 = mouseDownPos.y;
        maskBoxes[3].y0 = mouseDownPos.y;
        maskBoxes[3].y1 = pos.y;
        maskBoxes[4].y0 = pos.y;
        maskBoxes[4].y1 = h;
        maskBoxes[5].y0 = pos.y;
        maskBoxes[5].y1 = h;
        maskBoxes[6].y0 = pos.y;
        maskBoxes[6].y1 = h;
        maskBoxes[7].y0 = mouseDownPos.y;
        maskBoxes[7].y1 = pos.y;
    }
}
