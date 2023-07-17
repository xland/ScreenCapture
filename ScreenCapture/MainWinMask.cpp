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
    setCursor(cursor);
}

void MainWin::setMasks(const POINT& pos, const POINT& anotherPos)
{
    if (pos.x < anotherPos.x)
    {
        maskBoxes[0].x0 = 0;
        maskBoxes[0].x1 = pos.x;
        maskBoxes[1].x0 = pos.x;
        maskBoxes[1].x1 = anotherPos.x;
        maskBoxes[2].x0 = anotherPos.x;
        maskBoxes[2].x1 = w;
        maskBoxes[3].x0 = anotherPos.x;
        maskBoxes[3].x1 = w;
        maskBoxes[4].x0 = anotherPos.x;
        maskBoxes[4].x1 = w;
        maskBoxes[5].x0 = pos.x;
        maskBoxes[5].x1 = anotherPos.x;
        maskBoxes[6].x0 = 0;
        maskBoxes[6].x1 = pos.x;
        maskBoxes[7].x0 = 0;
        maskBoxes[7].x1 = pos.x;
        cutBox.x0 = pos.x;
        cutBox.x1 = anotherPos.x;
    }
    else
    {
        maskBoxes[0].x0 = 0;
        maskBoxes[0].x1 = anotherPos.x;
        maskBoxes[1].x0 = anotherPos.x;
        maskBoxes[1].x1 = pos.x;
        maskBoxes[2].x0 = pos.x;
        maskBoxes[2].x1 = w;
        maskBoxes[3].x0 = pos.x;
        maskBoxes[3].x1 = w;
        maskBoxes[4].x0 = pos.x;
        maskBoxes[4].x1 = w;
        maskBoxes[5].x0 = anotherPos.x;
        maskBoxes[5].x1 = pos.x;
        maskBoxes[6].x0 = 0;
        maskBoxes[6].x1 = anotherPos.x;
        maskBoxes[7].x0 = 0;
        maskBoxes[7].x1 = anotherPos.x;
        cutBox.x1 = pos.x;
        cutBox.x0 = anotherPos.x;
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
        maskBoxes[3].y1 = anotherPos.y;
        maskBoxes[4].y0 = anotherPos.y;
        maskBoxes[4].y1 = h;
        maskBoxes[5].y0 = anotherPos.y;
        maskBoxes[5].y1 = h;
        maskBoxes[6].y0 = anotherPos.y;
        maskBoxes[6].y1 = h;
        maskBoxes[7].y0 = pos.y;
        maskBoxes[7].y1 = anotherPos.y;
        cutBox.y0 = pos.y;
        cutBox.y1 = anotherPos.y;
    }
    else
    {
        maskBoxes[0].y0 = 0;
        maskBoxes[0].y1 = anotherPos.y;
        maskBoxes[1].y0 = 0;
        maskBoxes[1].y1 = anotherPos.y;
        maskBoxes[2].y0 = 0;
        maskBoxes[2].y1 = anotherPos.y;
        maskBoxes[3].y0 = anotherPos.y;
        maskBoxes[3].y1 = pos.y;
        maskBoxes[4].y0 = pos.y;
        maskBoxes[4].y1 = h;
        maskBoxes[5].y0 = pos.y;
        maskBoxes[5].y1 = h;
        maskBoxes[6].y0 = pos.y;
        maskBoxes[6].y1 = h;
        maskBoxes[7].y0 = anotherPos.y;
        maskBoxes[7].y1 = pos.y;
        cutBox.y1 = pos.y;
        cutBox.y0 = anotherPos.y;
    }
    InvalidateRect(hwnd, nullptr, false);
}

void MainWin::setCutBox(const POINT& pos)
{
    switch (mouseInMaskBoxIndex)
    {
        case 0: {
            POINT p;
            p.x = cutBox.x1;
            p.y = cutBox.y1;
            setMasks(pos, p);
            break;
        }
    }
}