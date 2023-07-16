#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
}
void MainWin::rightBtnDown(const POINT& pos)
{
    //canvasImage->writeToFile("123.png");
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    if (state == State::Start) {
        setMask(pos);
    }
    else if (state == State::Pen) {
        drawPen(pos);
    }
    else if (state == State::Eraser) {        
        drawEraser(pos);
    }
    InvalidateRect(hwnd, nullptr, false);
}
void MainWin::leftBtnUp(const POINT& pos)
{
    isLeftBtnDown = false;
    if (state == State::Start) {
        state = State::Eraser;
    }
    else if (state == State::Eraser) {
        
    }
}