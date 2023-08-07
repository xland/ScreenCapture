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
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 28;
            History::Push(shape);
            break;
        }
        case State::mosaic:
        {
            auto shape = new Shape::Mosaic();
            painter->bgImage->getData(shape->bgImgData);
            painter->canvasImage->getData(shape->canvasImgData);
            shape->screenH = painter->h;
            shape->screenW = painter->w;
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 8;
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
    isLeftBtnDown = true;
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
        else if (mouseEnterMainToolIndex == 11) //save
        {
            History::LastShapeDrawEnd();
            saveFile();
            return;
        }
        else if (mouseEnterMainToolIndex == 12) //clipboard
        {
            History::LastShapeDrawEnd();
            saveClipboard();
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
            //Debug("111 state:" + std::to_string((int)state));
            //设置几个图形默认是否需要填充
            if (state == State::rect||state == State::ellipse||state == State::line) {
                isFill = false;
            }
            else if (state == State::arrow || state == State::number) {
                isFill = true;
            }
            else if (state == State::text) {
                ChangeCursor(IDC_IBEAM);
            }
            InvalidateRect(hwnd, nullptr, false);
            return;
        }
        //Debug("222 state:" + std::to_string((int)state));
        if (mouseEnterSubToolIndex != -1) {
            subToolBtnClick();
            return;
        }
        //Debug("333 state:" + std::to_string((int)state));
        leftBtnDownStartDraw();
        //Debug("444 state:" + std::to_string((int)state));
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
    quitApp(2);
}
void MainWin::mouseMove(const POINT& pos)
{ 
    painter->pixelX = pos.x;
    painter->pixelY = pos.y;
    if (isLeftBtnDown) {
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
            case State::rect:
            case State::ellipse:
            case State::arrow:
            case State::number:
            case State::line:
            case State::mosaic:
            case State::pen:
            case State::eraser:
            {
                //Debug("state:" + std::to_string((int)state));
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
            Debug("IDC_CROSS");
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
        if (state == State::maskReady) {   
            Debug("State::maskReady");
            checkMouseEnterMaskBox(pos);
        }
        else
        {
            if (state == State::lastPathDrag) {
                History::LastShapeMouseInDragger(pos);
            }
            else if (state == State::text) {
                ChangeCursor(IDC_IBEAM);
                History::LastShapeMouseInDragger(pos);
            }
            else
            {
                ChangeCursor(IDC_CROSS);
            }
        }
        checkMouseEnterToolBox(pos);
    }

}
void MainWin::leftBtnUp(const POINT& pos)
{
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
        case State::mosaic:
        {
            History::LastShapeShowDragger();
            state = State::lastPathDrag;
            break;
        }
        case State::pen: 
        case State::text:
        {
            History::LastShapeShowDragger();
            break;
        }
        case State::eraser:
        {
            break;
        }
    }
}