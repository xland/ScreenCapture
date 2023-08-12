#include "MainWin.h"
#include <chrono>


void MainWin::leftBtnDownStartDraw()
{
    switch (state)
    {
        case State::start:
        {
            break;
        }
        case State::maskReady:
        {
            dragStartCutBoxStartPos = BLPoint(cutBox.x0, cutBox.y0);
            dragStartCutBoxEndPos = BLPoint(cutBox.x1, cutBox.y1);
            if (mouseInMaskBoxIndex < 8) {
                setCutBox(MouseDownPos);
                InvalidateRect(hwnd, nullptr, false);
            }
            return;
        }
        case State::box:
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
            //Debug("start pen");
            auto shape = new Shape::Pen();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(shape);
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
            shape->SetNumber();
            preState = state;
            painter->isDrawing = true;
            break;
        }
        case State::eraser:
        {
            auto shape = new Shape::Eraser();
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 36;
            History::Push(shape);
            break;
        }
        case State::mosaic:
        {
            auto shape = new Shape::Mosaic();
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 6;
            shape->isFill = true;
            shape->InitMosaicImg();
            History::Push(shape);
            preState = state;
            painter->isDrawing = true;
            break;
        }
        case State::text:
        {
            auto shape = new Shape::Text();
            shape->color = colors[colorBtnIndex];
            History::Push(shape);
            shape->Draw(MouseDownPos.x, MouseDownPos.y, -1, -1);
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


void MainWin::leftBtnDown(const POINT& pos)
{
    IsLeftBtnDown = true;
    MouseDownPos = pos;
    if (state != State::start) {
        if (mouseEnterMainToolIndex == 9) //undo
        {
            History::Undo();
            return;
        }
        else if (mouseEnterMainToolIndex == 10) //redo
        {
            History::Redo();
            return;
        }
        else if (mouseEnterMainToolIndex == 11) //save file
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            saveFile();
            return;
        }
        else if (mouseEnterMainToolIndex == 12) //save clipboard
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
            return;
        }
        else if (mouseEnterMainToolIndex == 13) //close
        {
            quitApp(1);
            return;
        }
        if (!History::LastShapeDrawEnd()) { 
            //draggerIndex == -1时返回false
            //text shape也会在这里改变光标位置
            return;
        }
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {            
            selectedToolIndex = mouseEnterMainToolIndex;
            state = (State)(selectedToolIndex+2); 
            //设置几个图形默认是否需要填充
            if (state == State::box||
                state == State::ellipse||
                state == State::line) 
            {
                isFill = false;
            }
            else if (state == State::arrow || 
                state == State::number ||
                state == State::mosaic
                ) 
            {
                isFill = true;
            }
            else if (state == State::text) 
            {
                ChangeCursor(IDC_IBEAM);
            }
            InvalidateRect(hwnd, nullptr, false);
            return;
        }
        if (mouseEnterSubToolIndex != -1) {
            subToolBtnClick();
            return;
        }
        leftBtnDownStartDraw();
    }
}
void MainWin::rightBtnDown(const POINT& pos)
{
    MouseDownPos = pos;
    if (painter->isDrawing)
    {
        History::LastShapeDrawEnd();
        return;
    }
    quitApp(2);
}
void MainWin::mouseMove(const POINT& pos)
{ 
    painter->pixelX = pos.x;
    painter->pixelY = pos.y;
    if (IsLeftBtnDown) {
        switch (state)
        {
            case State::start:
            {
                BLPoint startPos(pos.x, pos.y);
                BLPoint endPos(MouseDownPos.x, MouseDownPos.y);
                setCutBox(startPos, endPos);
                InvalidateRect(hwnd, nullptr, false);
                break;
            }
            case State::maskReady:
            {
                setCutBox(pos);
                InvalidateRect(hwnd, nullptr, false);
                break;
            }
            case State::box:
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
        if(state == State::start){
            ChangeCursor(IDC_CROSS);
            for (size_t i = 0; i < windowBoxes.size(); i++)
            {
                if (windowBoxes[i].contains(pos.x, pos.y)) {
                    BLPoint startPos(windowBoxes[i].x0, windowBoxes[i].y0);
                    BLPoint endPos(windowBoxes[i].x1, windowBoxes[i].y1);
                    setCutBox(startPos, endPos);
                    break;
                }
            }
            InvalidateRect(hwnd, nullptr, false);
            return;
        }
        if (checkMouseEnterToolBox(pos)) {
            return;
        }
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
    IsLeftBtnDown = false;
    if (mouseEnterMainToolIndex != -1 || mouseEnterSubToolIndex != -1) {
        InvalidateRect(hwnd, nullptr, false); //undo redo 的按钮状态
        return;
    }
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
        case State::box:
        case State::ellipse:
        case State::arrow:
        case State::lastPathDrag:
        case State::line:
        case State::number:
        case State::text:
        {
            History::LastShapeShowDragger();
            state = State::lastPathDrag;
            break;
        }
        case State::mosaic:
        {
            History::LastShapeShowDragger();
            auto shape = (Shape::Mosaic*)History::GetLastDrawShape();
            if (shape->isFill) {
                state = State::lastPathDrag;
            }
            else
            {
                shape->EndDraw();
            }            
            break;
        }
        case State::eraser:
        case State::pen: 
        {
            History::LastShapeShowDragger();
            InvalidateRect(hwnd, nullptr, false);
            break;
        }
    }
}