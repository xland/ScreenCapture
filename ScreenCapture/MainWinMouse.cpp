#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
}
void MainWin::rightBtnDown(const POINT& pos)
{
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    if (state == State::Start) {
        if (pos.x < mouseDownPos.x)
        {
            cutRect.left = pos.x;
            cutRect.right = mouseDownPos.x;
        }
        else
        {
            cutRect.right = pos.x;
            cutRect.left = mouseDownPos.x;
        }
        if (pos.y < mouseDownPos.y)
        {
            cutRect.top = pos.y;
            cutRect.bottom = mouseDownPos.y;
        }
        else
        {
            cutRect.top = mouseDownPos.y;
            cutRect.bottom = pos.y;
        }
    }
    else if (state == State::Eraser) {

        mouseDownPos = pos;
    }
    paint();
    InvalidateRect(hwnd, nullptr, false);
}
void MainWin::leftBtnUp(const POINT& pos)
{
    isLeftBtnDown = false;
    if (state == State::Start) {
        state = State::Eraser;

    }
}