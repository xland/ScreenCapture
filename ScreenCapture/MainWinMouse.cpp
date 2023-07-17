#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
    if (state != State::Start) {
        if (mouseEnterToolIndex != -1 && mouseEnterToolIndex < 9) {
            selectedToolIndex = mouseEnterToolIndex;
            InvalidateRect(hwnd, nullptr, false);
            return;
        }
        if (state == State::MaskReady) {
            setCutBox(pos);
            return;
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
        switch (state)
        {
            case State::Start:
            {
                setMasks(pos,mouseDownPos);
                break;
            }
            case State::MaskReady:
            {
                //setCutBox(pos);
                break;
            }
            case State::RectEllipse:
                break;
            case State::Arrow:
                break;
            case State::Pen:
            {
                drawPen(pos);
                break;
            }
            case State::Mosaic:
                break;
            case State::Text:
                break;
            case State::Eraser:
            {
                drawEraser(pos);
                break;
            }
            case State::Number:
                break;
            case State::LastPathDrag:
                break;
            default:
                break;
        }
    }
    else
    {
        if (state != State::Start) {
            checkMouseEnterToolBox(pos);
            if (mouseEnterToolIndex == -1) {
                checkMouseEnterMaskBox(pos);
            }            
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