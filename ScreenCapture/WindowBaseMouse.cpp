#include "WindowBase.h"
#include <chrono>

void WindowBase::leftBtnDownStartDraw()
{
    switch (state)
    {
    case State::start:
    {
        break;
    }
    case State::maskReady:
    {
        SetCutBoxWhenLeftButtonDown();
        return;
    }
    case State::box:
    {
        auto box = new Shape::Box();
        box->color = colors[colorBtnIndex];
        box->IsFill = IsFill;
        box->strokeWidth = strokeWidths[strokeBtnIndex];
        History::Push(box);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::ellipse:
    {
        auto ellipse = new Shape::Ellipse();
        ellipse->color = colors[colorBtnIndex];
        ellipse->IsFill = IsFill;
        ellipse->strokeWidth = strokeWidths[strokeBtnIndex];
        History::Push(ellipse);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::arrow:
    {
        auto arrow = new Shape::Arrow();
        arrow->color = colors[colorBtnIndex];
        arrow->IsFill = IsFill;
        arrow->strokeWidth = strokeWidths[strokeBtnIndex];
        History::Push(arrow);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::pen:
    {
        //Debug("start pen");
        auto shape = new Shape::Pen();
        shape->color = colors[colorBtnIndex];
        shape->strokeWidth = strokeWidths[strokeBtnIndex];
        History::Push(shape);
        IsDrawing = true;
        break;
    }
    case State::line:
    {
        auto shape = new Shape::Line();
        shape->color = colors[colorBtnIndex];
        shape->strokeWidth = strokeWidths[strokeBtnIndex] + 26;
        shape->IsFill = IsFill;
        History::Push(shape);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::number:
    {
        auto shape = new Shape::Number();
        shape->color = colors[colorBtnIndex];
        shape->strokeWidth = strokeWidths[strokeBtnIndex];
        shape->IsFill = IsFill;
        History::Push(shape);
        shape->SetNumber();
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::eraser:
    {
        auto shape = new Shape::Eraser();
        shape->strokeWidth = strokeWidths[strokeBtnIndex] + 36;
        shape->IsFill = IsFill;
        shape->CopyCanvasImg();
        History::Push(shape);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::mosaic:
    {
        auto shape = new Shape::Mosaic();
        shape->strokeWidth = strokeWidths[strokeBtnIndex] + 6;
        shape->IsFill = IsFill;
        shape->InitMosaicImg();
        History::Push(shape);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::text:
    {
        auto shape = new Shape::Text();
        shape->color = colors[colorBtnIndex];
        History::Push(shape);
        shape->Draw(MouseDownPos.x, MouseDownPos.y, -1, -1);
        SetTimer(hwnd, 999, 660, (TIMERPROC)NULL);
        PreState = state;
        IsDrawing = true;
        break;
    }
    case State::lastPathDrag:
    {
        break;
    }
    }
}
bool WindowBase::OnLeftButtonDown(const int& x, const int& y)
{
    MouseDownPos.x = x;
    MouseDownPos.y = y;
    if (state >= State::maskReady) {
        static auto t1 = std::chrono::system_clock::now();
        auto t2 = std::chrono::system_clock::now();
        auto count = floor<std::chrono::milliseconds>(t2 - t1).count();
        if (count > 0 && count < 260 && mouseEnterMainToolIndex == -1 && mouseEnterSubToolIndex == -1) {
            IsDoubleClick = true;
            if (!IsDrawing) {
                saveClipboard();
                return false;
            }
            else
            {
                auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
                if (shape) {
                    shape->EndDraw();
                }
                SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
                return false;
            }
        }
        t1 = t2;
    }
    if (state != State::start) {
        if (mouseEnterMainToolIndex == 9) //undo
        {
            IsLeftBtnDown = false;
            History::Undo();
            return false;
        }
        else if (mouseEnterMainToolIndex == 10) //redo
        {
            IsLeftBtnDown = false;
            History::Redo();
            return false;
        }
        else if (mouseEnterMainToolIndex == 11 && IsMainWin) //pin window
        {
            IsLeftBtnDown = false;
            if (IsMainWin) {
                PinWindow();
            }            
            return false;
        }
        else if (mouseEnterMainToolIndex == 12) //save file
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            SaveFile();
            return false;
        }
        else if (mouseEnterMainToolIndex == 13) //save clipboard
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
            return false;
        }
        else if (mouseEnterMainToolIndex == 14) //close
        {
            IsLeftBtnDown = false;
            quitApp(1);
            return false;
        }
        if (!History::LastShapeDrawEnd()) {
            //draggerIndex == -1时返回false
            //text shape也会在这里改变光标位置
            return false;
        }
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {
            if (SelectedToolIndex == mouseEnterMainToolIndex) {
                state = State::maskReady;
                SelectedToolIndex = -1;
                Refresh();
                return false;
            }
            SelectedToolIndex = mouseEnterMainToolIndex;
            state = (State)(SelectedToolIndex + 2);
            //设置几个图形默认是否需要填充
            if (state == State::box || state == State::ellipse ||
                state == State::line || state == State::eraser)
            {
                IsFill = false;
            }
            else if (state == State::arrow || state == State::number || state == State::mosaic)
            {
                IsFill = true;
            }
            else if (state == State::text)
            {
                ChangeCursor(IDC_IBEAM);
            }
            Refresh();
            return false;
        }
        if (mouseEnterSubToolIndex != -1) {
            IsLeftBtnDown = false;
            subToolBtnClick();
            return false;
        }
        leftBtnDownStartDraw();
    }
    return false;
}
bool WindowBase::OnRightButtonDown(const int& x, const int& y)
{
    MouseDownPos.x = x;
    MouseDownPos.y = y;
    if (IsDrawing)
    {
        History::LastShapeDrawEnd();
        return false;
    }
    quitApp(2);
    return false;
}
bool WindowBase::OnMouseMove(const int& x, const int& y)
{
    pixelX = x;
    pixelY = y;
    if (IsLeftBtnDown) {
        switch (state)
        {
        case State::start:
        {
            SetCutBox(x, y, MouseDownPos.x, MouseDownPos.y);
            break;
        }
        case State::maskReady:
        {
            SetCutBox(x, y);
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
            History::LastShapeDraw(POINT{ .x{x},.y{y} }, MouseDownPos,state);
            break;
        }
        case State::text:
        case State::lastPathDrag:
        {
            History::LastShapeDragDragger(x,y);
            break;
        }
        default:
            break;
        }
    }
    else {
        if (state == State::start) {
            ChangeCursor(IDC_CROSS);
            HighLightWindowBox(x, y);
            return false;
        }
        if (checkMouseEnterToolBox(x,y) ){
            return false;
        }
        if (state == State::maskReady) {
            MouseMoveWhenMaskReady(x,y);
        }
        else
        {
            if (state == State::lastPathDrag) {
                History::LastShapeMouseInDragger(POINT{ .x{x},.y{y} });
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
    return false;
}
bool WindowBase::OnLeftButtonUp(const int& x, const int& y)
{
    if (mouseEnterMainToolIndex != -1 || mouseEnterSubToolIndex != -1) {
        Refresh();
        return false;
    }
    switch (state)
    {
    case State::start:
    {
        state = State::maskReady;
        Refresh();
        break;
    }
    case State::maskReady:
    {
        Refresh();
        break;
    }
    case State::box:
    case State::ellipse:
    case State::arrow:
    case State::lastPathDrag:
    case State::line:
    case State::number:
    case State::text:
    case State::mosaic:
    {
        History::LastShapeShowDragger();
        state = State::lastPathDrag;
        break;
    }
    case State::eraser:
    case State::pen:
    {
        History::LastShapeShowDragger();
        Refresh();
        break;
    }
    }
    return false;
}