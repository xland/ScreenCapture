#include "MainWin.h"


void MainWin::setMasks(const POINT& pos)
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
        cutBox.x0 = pos.x;
        cutBox.x1 = mouseDownPos.x;
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
        cutBox.x1 = pos.x;
        cutBox.x0 = mouseDownPos.x;
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
        cutBox.y0 = pos.y;
        cutBox.y1 = mouseDownPos.y;
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
        cutBox.y1 = pos.y;
        cutBox.y0 = mouseDownPos.y;
    }
}