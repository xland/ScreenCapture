#include "MainWin.h"


void MainWin::checkMouseEnterMaskBox(const POINT& pos) {
    int index = -1;
    for (size_t i = 0; i < 8; i++)
    {
        if (maskBoxes[i].contains(pos.x, pos.y)) {
            index = i;
            break;
        }
    }
    if (index == -1) index = 8;
    if (mouseInMaskBoxIndex == index) return;
    mouseInMaskBoxIndex = index;
    LPCTSTR cursor = IDC_SIZENWSE;
    switch (index)
    {
        case 1:
        case 5:
        {
            cursor = IDC_SIZENS;
            break;
        }
        case 2:
        case 6:
        {
            cursor = IDC_SIZENESW;
            break;
        }
        case 3:
        case 7:
        {
            cursor = IDC_SIZEWE;
            break;
        }
        case 8:
        {
            cursor = IDC_SIZEALL;
            break;
        }
    }
    ChangeCursor(cursor);
}

void MainWin::setCutBox(BLPoint& startPos, BLPoint& endPos)
{
    SetBoxByPos(cutBox, startPos.x,startPos.y, endPos.x,endPos.y);
    maskBoxes[0].x0 = 0;
    maskBoxes[0].x1 = cutBox.x0;
    maskBoxes[1].x0 = cutBox.x0;
    maskBoxes[1].x1 = cutBox.x1;
    maskBoxes[2].x0 = cutBox.x1;
    maskBoxes[2].x1 = painter->w;
    maskBoxes[3].x0 = cutBox.x1;
    maskBoxes[3].x1 = painter->w;
    maskBoxes[4].x0 = cutBox.x1;
    maskBoxes[4].x1 = painter->w;
    maskBoxes[5].x0 = cutBox.x0;
    maskBoxes[5].x1 = cutBox.x1;
    maskBoxes[6].x0 = 0;
    maskBoxes[6].x1 = cutBox.x0;
    maskBoxes[7].x0 = 0;
    maskBoxes[7].x1 = cutBox.x0;

    maskBoxes[0].y0 = 0;
    maskBoxes[0].y1 = cutBox.y0;
    maskBoxes[1].y0 = 0;
    maskBoxes[1].y1 = cutBox.y0;
    maskBoxes[2].y0 = 0;
    maskBoxes[2].y1 = cutBox.y0;
    maskBoxes[3].y0 = cutBox.y0;
    maskBoxes[3].y1 = cutBox.y1;
    maskBoxes[4].y0 = cutBox.y1;
    maskBoxes[4].y1 = painter->h;
    maskBoxes[5].y0 = cutBox.y1;
    maskBoxes[5].y1 = painter->h;
    maskBoxes[6].y0 = cutBox.y1;
    maskBoxes[6].y1 = painter->h;
    maskBoxes[7].y0 = cutBox.y0;
    maskBoxes[7].y1 = cutBox.y1;
}


void MainWin::setCutBox(const POINT& pos)
{
    switch (mouseInMaskBoxIndex)
    {
        case 0: {            
            dragStartCutBoxStartPos.x = pos.x;
            dragStartCutBoxStartPos.y = pos.y;
            break;
        }
        case 1: {
            dragStartCutBoxStartPos.y = pos.y;
            break;
        }
        case 2: {
            dragStartCutBoxStartPos.y = pos.y;
            dragStartCutBoxEndPos.x = pos.x;
            break;
        }
        case 3: {
            dragStartCutBoxEndPos.x = pos.x;
            break;
        }
        case 4: {
            dragStartCutBoxEndPos.x = pos.x;
            dragStartCutBoxEndPos.y = pos.y;
            break;
        }
        case 5: {
            dragStartCutBoxEndPos.y = pos.y;
            break;
        }
        case 6: {
            dragStartCutBoxEndPos.y = pos.y;
            dragStartCutBoxStartPos.x = pos.x;
            break;
        }
        case 7: {
            dragStartCutBoxStartPos.x = pos.x;
            break;
        }
        case 8: {
            auto xSpan = pos.x - MouseDownPos.x;
            auto ySpan = pos.y - MouseDownPos.y;
            dragStartCutBoxStartPos.x += xSpan;
            dragStartCutBoxStartPos.y += ySpan;
            dragStartCutBoxEndPos.x += xSpan;
            dragStartCutBoxEndPos.y += ySpan;
            MouseDownPos = pos;
        }
    }
    setCutBox(dragStartCutBoxStartPos, dragStartCutBoxEndPos);
}