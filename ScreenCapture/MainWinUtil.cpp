#include "MainWin.h"

void MainWin::setBoxByPos(BLBox& box,const BLPoint& startPos,const BLPoint& endPos)
{
    if (startPos.x < endPos.x) {
        box.x0 = startPos.x;
        box.x1 = endPos.x;
    }
    else
    {
        box.x0 = endPos.x;
        box.x1 = startPos.x;
    }
    if (startPos.y < endPos.y) {
        box.y0 = startPos.y;
        box.y1 = endPos.y;
    }
    else
    {
        box.y0 = endPos.y;
        box.y1 = startPos.y;
    }
}