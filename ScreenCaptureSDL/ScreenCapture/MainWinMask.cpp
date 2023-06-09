#include "MainWin.h"

void MainWin::createMaskByMouse(const SDL_Point& pos)
{
    if (pos.x < mouseDownPos.x) {
        maskRect[0].w = pos.x;
        maskRect[1].x = pos.x;
        maskRect[1].w = mouseDownPos.x - pos.x;
        maskRect[2].x = mouseDownPos.x;
        maskRect[2].w = screenRect.w - mouseDownPos.x;
        maskRect[3].x = mouseDownPos.x;
        maskRect[3].w = screenRect.w - mouseDownPos.x;
        maskRect[4].x = mouseDownPos.x;
        maskRect[4].w = screenRect.w - mouseDownPos.x;
        maskRect[5].x = pos.x;
        maskRect[5].w = mouseDownPos.x - pos.x;
        maskRect[6].w = pos.x;
        maskRect[7].w = pos.x;
        cutRect.x = pos.x;
        cutRect.w = mouseDownPos.x - pos.x;
    }
    else
    {
        maskRect[0].w = mouseDownPos.x;
        maskRect[1].x = mouseDownPos.x;
        maskRect[1].w = pos.x - mouseDownPos.x;
        maskRect[2].x = pos.x;
        maskRect[2].w = screenRect.w - pos.x;
        maskRect[3].x = pos.x;
        maskRect[3].w = screenRect.w - pos.x;
        maskRect[4].x = pos.x;
        maskRect[4].w = screenRect.w - pos.x;
        maskRect[5].x = mouseDownPos.x;
        maskRect[5].w = pos.x - mouseDownPos.x;
        maskRect[6].w = mouseDownPos.x;
        maskRect[7].w = mouseDownPos.x;
        cutRect.x = mouseDownPos.x;
        cutRect.w = pos.x - mouseDownPos.x;
    }
    if (pos.y < mouseDownPos.y) {
        maskRect[0].h = pos.y;
        maskRect[1].h = pos.y;
        maskRect[2].h = pos.y;
        maskRect[3].y = pos.y;
        maskRect[3].h = mouseDownPos.y - pos.y;
        maskRect[4].y = mouseDownPos.y;
        maskRect[4].h = screenRect.h - mouseDownPos.y;
        maskRect[5].y = mouseDownPos.y;
        maskRect[5].h = screenRect.h - mouseDownPos.y;
        maskRect[6].y = mouseDownPos.y;
        maskRect[6].h = screenRect.h - mouseDownPos.y;
        maskRect[7].y = pos.y;
        maskRect[7].h = mouseDownPos.y - pos.y;
        cutRect.y = pos.y;
        cutRect.h = mouseDownPos.y - pos.y;
    }
    else
    {
        maskRect[0].h = mouseDownPos.y;
        maskRect[1].h = mouseDownPos.y;
        maskRect[2].h = mouseDownPos.y;
        maskRect[3].y = mouseDownPos.y;
        maskRect[3].h = pos.y- mouseDownPos.y;
        maskRect[4].y = pos.y;
        maskRect[4].h = screenRect.h - pos.y;
        maskRect[5].y = pos.y;
        maskRect[5].h = screenRect.h - pos.y;
        maskRect[6].y = pos.y;
        maskRect[6].h = screenRect.h - pos.y;
        maskRect[7].y = mouseDownPos.y;
        maskRect[7].h = pos.y - mouseDownPos.y;
        cutRect.y = mouseDownPos.y;
        cutRect.h = pos.y - mouseDownPos.y;
    }
    render();

}