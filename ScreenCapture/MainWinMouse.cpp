#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
    if (state != State::Start) {
        if (mouseEnterToolIndex != -1 && mouseEnterToolIndex < 9) {
            selectedToolIndex = mouseEnterToolIndex;
            InvalidateRect(hwnd, nullptr, false);
        }
    }
}
void MainWin::rightBtnDown(const POINT& pos)
{
    //canvasImage->writeToFile("123.png");
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (isLeftBtnDown)
    {
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
    else
    {
        if (state != State::Start) {
            checkMouseEnterToolBox(pos);
        }
    }

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