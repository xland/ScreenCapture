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
        setMasks(pos);
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
    if (!isLeftBtnDown) return;
    isLeftBtnDown = false;
    if (state == State::Start) {
        state = State::MaskReady;
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (state == State::Eraser) {
        
    }
}