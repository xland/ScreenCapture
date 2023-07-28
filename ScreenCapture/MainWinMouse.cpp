#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
    if (state != State::start) {
        endDrawing();
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {
            selectedToolIndex = mouseEnterMainToolIndex;
            InvalidateRect(hwnd, nullptr, false);
            state = (State)(selectedToolIndex+2);
            return;
        }
        if (mouseEnterSubToolIndex != -1) {
            subToolBtnClick();
            return;
        }
        if (state == State::maskReady) {

        }
        switch (state)
        {
        case State::start:
            break;
        case State::maskReady:
        {
            dragStartCutBoxStartPos = BLPoint(cutBox.x0, cutBox.y0);
            dragStartCutBoxEndPos = BLPoint(cutBox.x1, cutBox.y1);
            if (mouseInMaskBoxIndex < 8) {
                setCutBox(pos);
            }
            return;
        }
        case State::rect:
        {
            auto box = new Shape::Box();
            box->color = colors[colorBtnIndex];
            box->isFill = isFill;
            box->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(box);
            break;
        }
        case State::ellipse:
        {
            auto ellipse = new Shape::Ellipse();
            ellipse->color = colors[colorBtnIndex];
            ellipse->isFill = isFill;
            ellipse->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(ellipse);
            break;
        }
        case State::arrow:
        {
            auto arrow = new Shape::Arrow();
            arrow->color = colors[colorBtnIndex];
            arrow->isFill = isFill;
            arrow->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(arrow);
            break;
        }
        case State::pen:
        {
            auto shape = new Shape::Pen();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(shape);
            break;
        }
        case State::line:
        {
            auto shape = new Shape::Line();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(shape);
            break;
        }
        case State::number:
        {
            auto shape = new Shape::Number();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            shape->isFill = isFill;
            History::Push(shape);
            break;
        }
        case State::mosaic:
        {
            break;
        }
        case State::text:
        {
            auto shape = new Shape::Text();
            shape->color = colors[colorBtnIndex];
            History::Push(shape);
            break;
        }
        case State::eraser:
        {
            break;
        }
        case State::lastPathDrag:
        {
            break;
        }
        }
        preState = state;
        isDrawing = true;
    }
}
void MainWin::rightBtnDown(const POINT& pos)
{
    //canvasImage->writeToFile("123.png");
    if (endDrawing()) return;
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{   
    if (isLeftBtnDown) {
        switch (state)
        {
            case State::start:
            {
                BLPoint startPos(pos.x, pos.y);
                BLPoint endPos(mouseDownPos.x, mouseDownPos.y);
                setCutBox(startPos, endPos);
                break;
            }
            case State::maskReady:
            {
                setCutBox(pos);
                break;
            }
            case State::rect:
            case State::ellipse:
            case State::arrow:
            case State::number:
            {
                drawShape(pos);
                break;
            }
            case State::pen:
            {
                drawPen(pos);
                break;
            }
            case State::mosaic:
                break;
            case State::text:
                break;
            case State::eraser:
            {
                drawEraser(pos);
                break;
            }
            case State::lastPathDrag:
                break;
            default:
                break;
        }
    }
    else {
        if (checkMouseEnterToolBox(pos)) return;
        if (state == State::maskReady) {            
            checkMouseEnterMaskBox(pos);
        }
        else
        {
            setCursor(IDC_CROSS);
        }
    }

}
void MainWin::leftBtnUp(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    isLeftBtnDown = false;
    if (mouseEnterMainToolIndex != -1 || mouseEnterSubToolIndex != -1) return;
    switch (state)
    {
        case State::start:
        {
            state = State::maskReady;
            InvalidateRect(hwnd, nullptr, false);
            break;
        }
        case State::maskReady:
        {
            break;
        }
        case State::rect:
        {
            state = State::lastPathDrag;
            break;
        }
        case State::ellipse:
        {
            state = State::lastPathDrag;
            break;
        }
        case State::arrow:
        {
            state = State::lastPathDrag;
            break;
        }
        case State::pen:
            break;
        case State::line:
            break;
        case State::mosaic:
            break;
        case State::text:
            break;
        case State::number:
            break;
        case State::eraser:
            break;
        case State::lastPathDrag:
            break;
    }
}