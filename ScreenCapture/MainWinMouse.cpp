#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
}
void MainWin::rightBtnDown(const POINT& pos)
{
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    if (pos.x < mouseDownPos.x) {
        masks[0].right = pos.x+0.5;
        masks[1].left = pos.x;
        masks[1].right = mouseDownPos.x;
        masks[2].left = mouseDownPos.x;
        masks[2].right = w;
        masks[3].left = mouseDownPos.x;
        masks[3].right = w;
        masks[4].left = mouseDownPos.x;
        masks[4].right = w;
        masks[5].left = pos.x;
        masks[5].right = mouseDownPos.x;
        masks[6].right = pos.x;
        masks[7].right = pos.x;
        cutRect.left = pos.x;
        cutRect.right = mouseDownPos.x;
    }
    else
    {
        masks[0].right = mouseDownPos.x;
        masks[1].left = mouseDownPos.x;
        masks[1].right = pos.x;
        masks[2].left = pos.x;
        masks[2].right = w;
        masks[3].left = pos.x;
        masks[3].right = w;
        masks[4].left = pos.x;
        masks[4].right = w;
        masks[5].left = mouseDownPos.x;
        masks[5].right = pos.x;
        masks[6].right = mouseDownPos.x;
        masks[7].right = mouseDownPos.x;
        cutRect.left = mouseDownPos.x;
        cutRect.right = pos.x;
    }
    if (pos.y < mouseDownPos.y) {
        masks[0].bottom = pos.y;
        masks[1].bottom = pos.y;
        masks[2].bottom = pos.y;
        masks[3].top = pos.y;
        masks[3].bottom = mouseDownPos.y;
        masks[4].bottom = h;
        masks[4].top = mouseDownPos.y;
        masks[5].top = mouseDownPos.y;
        masks[5].bottom = h;
        masks[6].top = mouseDownPos.y;
        masks[6].bottom = h;
        masks[7].top = pos.y;
        masks[7].bottom = mouseDownPos.y;
        cutRect.top = pos.y;
        cutRect.bottom = mouseDownPos.y;
    }
    else
    {
        masks[0].bottom = mouseDownPos.y;
        masks[1].bottom = mouseDownPos.y;
        masks[2].bottom = mouseDownPos.y;
        masks[3].top = mouseDownPos.y;
        masks[3].bottom = pos.y;
        masks[4].top = pos.y;
        masks[4].bottom = h;
        masks[5].top = pos.y;
        masks[5].bottom = h;
        masks[6].top = pos.y;
        masks[6].bottom = h;
        masks[7].top = mouseDownPos.y;
        masks[7].bottom = pos.y;
        cutRect.top = mouseDownPos.y;
        cutRect.bottom = pos.y;
    }
    InvalidateRect(hwnd, nullptr, false);
}
void MainWin::leftBtnUp(const POINT& pos)
{
    isLeftBtnDown = false;
}