#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    MouseDownPos = pos;
    if (state != State::start) {
        if (!History::LastShapeDrawEnd()) {
            return;
        }
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {
            selectedToolIndex = mouseEnterMainToolIndex;
            InvalidateRect(hwnd, nullptr, false);
            state = (State)(selectedToolIndex+2);
            if (state == State::rect||state == State::ellipse||state == State::line) {
                isFill = false;
            }
            else if (state == State::arrow || state == State::number) {
                isFill = true;
            }
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
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::ellipse:
            {
                auto ellipse = new Shape::Ellipse();
                ellipse->color = colors[colorBtnIndex];
                ellipse->isFill = isFill;
                ellipse->strokeWidth = strokeWidths[strokeBtnIndex];
                History::Push(ellipse);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::arrow:
            {
                auto arrow = new Shape::Arrow();
                arrow->color = colors[colorBtnIndex];
                arrow->isFill = isFill;
                arrow->strokeWidth = strokeWidths[strokeBtnIndex];
                History::Push(arrow);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::pen:
            {
                auto shape = new Shape::Pen();
                shape->color = colors[colorBtnIndex];
                shape->strokeWidth = strokeWidths[strokeBtnIndex];                
                History::Push(shape);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::line:
            {
                auto shape = new Shape::Line();
                shape->color = colors[colorBtnIndex];
                shape->strokeWidth = strokeWidths[strokeBtnIndex] + 26;
                shape->isFill = isFill;
                History::Push(shape);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::number:
            {
                auto shape = new Shape::Number();
                shape->color = colors[colorBtnIndex];
                shape->strokeWidth = strokeWidths[strokeBtnIndex];
                shape->isFill = isFill;
                History::Push(shape);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::eraser:
            {
                auto shape = new Shape::Eraser();
                shape->strokeWidth = strokeWidths[strokeBtnIndex]+28;
                History::Push(shape);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::mosaic:
            {
                auto shape = new Shape::Mosaic();
                painter->bgImage->getData(shape->bgImgData);
                painter->canvasImage->getData(shape->canvasImgData);
                shape->screenH = painter->h;
                shape->screenW = painter->w;
                shape->strokeWidth = strokeWidths[strokeBtnIndex]+8;
                History::Push(shape);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::text:
            {
                auto shape = new Shape::Text();
                shape->color = colors[colorBtnIndex];
                shape->hwnd = hwnd;
                History::Push(shape);
                shape->Draw(pos.x, pos.y, -1, -1);
                SetTimer(hwnd, 999, 660, (TIMERPROC)NULL);
                preState = state;
                painter->isDrawing = true;
                break;
            }
            case State::lastPathDrag:
            {
                break;
            }
        }
    }
}
void MainWin::rightBtnDown(const POINT& pos)
{
    //canvasImage->writeToFile("123.png");
    MouseDownPos = pos;
    if (painter->isDrawing)
    {
        History::LastShapeDrawEnd();
        return;
    }
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
                BLPoint endPos(MouseDownPos.x, MouseDownPos.y);
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
            case State::line:
            case State::mosaic:
            case State::pen:
            case State::eraser:
            {
                History::LastShapeDraw(pos,MouseDownPos);
                break;
            }
            case State::text:
                break;
            case State::lastPathDrag:
            {
                History::LastShapeDragDragger(pos);
                break;
            }
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
            if (state == State::lastPathDrag) {
                History::LastShapeMouseInDragger(pos);
            }
            else if (state == State::text) {
                ChangeCursor(IDC_IBEAM);
            }
            else
            {
                ChangeCursor(IDC_CROSS);
            }            
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
        case State::ellipse:
        case State::arrow:
        case State::lastPathDrag:
        case State::line:
        case State::number:
        {
            History::LastShapeShowDragger();
            state = State::lastPathDrag;
            break;
        }
        case State::pen:
        {
            state = State::lastPathDrag;
            break;
        }
        case State::mosaic:
            break;
        case State::text:
            break;
        case State::eraser:
            break;
    }
}