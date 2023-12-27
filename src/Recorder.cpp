#include "Recorder.h"
#include "Shape/ShapeBase.h"
#include "WindowMain.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapePen.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeEraser.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeDragger.h"

Recorder *recorder;

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
}

void Recorder::init()
{
    if (!recorder)
    {
        recorder = new Recorder();
    }
}

Recorder *Recorder::get()
{
    return recorder;
}

bool Recorder::OnMouseDown(const int &x, const int &y)
{
    auto win = WindowMain::get();
    if (win->state < State::rect)
    {
        return false;
    }
    if (curIndex < 0)
    {
        createShape(x, y, win->state);
        curIndex = shapes.size() - 1;
    }
    else {
        auto canvasBack = win->surfaceBack->getCanvas();
        canvasBack->clear(SK_ColorTRANSPARENT);
        for (auto& shape : shapes)
        {
            if (!shape->isWip) {
                shape->Paint(canvasBack);
            }            
        }
    }
    shapes[curIndex]->OnMouseDown(x, y);
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    if (curIndex < 0)
    {
        return false;
    }
    auto winMain = WindowMain::get();
    if (winMain->state == State::text) {
        return false;
    }
    // todo 
    //if (shapes[curIndex]->isWip)
    //{
    //    shapes.erase(shapes.begin() + curIndex, shapes.begin() + 1);
    //}
    shapes[curIndex]->OnMouseUp(x, y);
    curIndex = -1;
    return false;
}
bool Recorder::OnMouseMove(const int &x, const int &y)
{
    auto winMain = WindowMain::get();
    if (winMain->state == State::text) {
        return false;
    }
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->OnMouseMove(x, y);
        if (flag)
        {
            curIndex = i;
            break;
        }
        else
        {
            curIndex = -1;
        }
    }
    if (curIndex < 0)
    {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        Timer::get()->Start(0, 800, []() {
            ShapeDragger::get()->hideDragger();
            return true;
            });
    }
    return false;
}
bool Recorder::OnMouseDrag(const int &x, const int &y)
{
    if (curIndex >= 0)
    {
        shapes[curIndex]->OnMoseDrag(x, y);
    }
    return false;
}
bool Recorder::onChar(const unsigned int& val)
{
    shapes[curIndex]->OnChar(val);
    return false;    
}
bool Recorder::onKeyDown(const unsigned int& val)
{
    shapes[curIndex]->OnKeyDown(val);
    return false;
}


//bool Recorder::OnPaint(SkCanvas *canvas)
//{
//    for (auto &shape : shapes)
//    {
//        shape->OnPaint(canvas);
//    }
//    return false;
//}

//bool Recorder::OnPaintFinish(SkCanvas* canvas)
//{
//    for (auto& shape : shapes)
//    {
//        shape->OnPaintFinish(canvas);
//    }
//    return false;
//}

//bool Recorder::hideDragger(const int& id)
//{
//    auto win = WindowMain::get();
//    if (!win || !shapes[id]) {
//        return true;
//    }
//    if (id == curIndex) {
//        //auto func = std::bind(&Recorder::hideDragger, this, std::placeholders::_1);
//        //timer->Start(id, 800, func);
//        return false;
//    }
//    shapes[id]->showDragger = false;
//    PostMessage(WindowMain::get()->hwnd, WM_REFRESH, NULL, NULL);
//    return true;
//}

bool Recorder::flashTextCursor(const int& id)
{
    auto win = WindowMain::get();
    if (!win || !shapes[id]) {
        return true;
    }
    auto shapeText = dynamic_cast<ShapeText*>(shapes[id].get());
    shapeText->ShowCursor = !shapeText->ShowCursor;
    //auto func = std::bind(&Recorder::flashTextCursor, this, std::placeholders::_1);
    //timer->Start(id, 600, func);
    PostMessage(WindowMain::get()->hwnd, WM_REFRESH, NULL, NULL);
    return false;
}

void Recorder::createShape(const int &x, const int &y, const State &state)
{
    switch (state)
    {
    case State::rect:
    {
        shapes.push_back(std::make_shared<ShapeRect>(x, y));
        break;
    }
    case State::ellipse:
    {
        shapes.push_back(std::make_shared<ShapeEllipse>(x, y));
        break;
    }
    case State::arrow:
    {
        shapes.push_back(std::make_shared<ShapeArrow>(x, y));
        break;
    }
    case State::number:
    {
        shapes.push_back(std::make_shared<ShapeNumber>(x, y));
        break;
    }
    case State::pen:
    {
        shapes.push_back(std::make_shared<ShapePen>(x, y));
        break;
    }
    case State::line:
    {
        shapes.push_back(std::make_shared<ShapeLine>(x, y));
        break;
    }
    case State::text:
    {
        shapes.push_back(std::make_shared<ShapeText>(x, y));
        //auto func = std::bind(&Recorder::flashTextCursor, this, std::placeholders::_1);
        //timer->Start(shapes.size()-1, 600, func);
        break;
    }
    case State::mosaic:
    {
        shapes.push_back(std::make_shared<ShapeMosaic>(x, y));
        break;
    }
    case State::eraser:
    {
        shapes.push_back(std::make_shared<ShapeEraser>(x, y));
        break;
    }
    default:
        break;
    }
}