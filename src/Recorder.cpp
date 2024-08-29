#include "Recorder.h"
#include "App.h"
#include "WinBase.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
//#include "Shape/ShapeEllipse.h"
//#include "Shape/ShapeArrow.h"
//#include "Shape/ShapeNumber.h"
//#include "Shape/ShapePen.h"
//#include "Shape/ShapeLine.h"
//#include "Shape/ShapeText.h"
//#include "Shape/ShapeEraser.h"
//#include "Shape/ShapeEraserRect.h"
//#include "Shape/ShapeMosaic.h"
//#include "Shape/ShapeMosaicRect.h"

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
}

void Recorder::Init()
{
    listenLeftBtnDown(std::bind(&Recorder::OnLeftBtnDown, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseMove(std::bind(&Recorder::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2));
    listenCustomMsg(std::bind(&Recorder::OnCustomMsg, this, std::placeholders::_1, std::placeholders::_2));
    listenPaint(std::bind(&Recorder::OnPaint, this, std::placeholders::_1));
}

void Recorder::OnLeftBtnDown(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state < State::rect) {
        return;
    }
    createShape(x, y, win->state);
    curShape = &shapes.back();
    curShape->OnMouseDown(x, y);
}

void Recorder::OnCustomMsg(const EventType& type, const uint32_t& msg)
{
}

void Recorder::OnMouseMove(const int& x, const int& y)
{
}

void Recorder::OnPaint(SkCanvas* canvas)
{
}

void Recorder::createShape(const int& x, const int& y, const State& state)
{
    switch (state)
    {
    case State::rect:
    {
        shapes.push_back(ShapeRect(x, y));
        break;
    }
    /*case State::ellipse:
    {
        shapes.push_back(ShapeEllipse(x, y));
        break;
    }
    case State::arrow:
    {
        shapes.push_back(ShapeArrow(x, y));
        break;
    }
    case State::number:
    {
        shapes.push_back(ShapeNumber(x-36, y-36));
        break;
    }
    case State::pen:
    {
        shapes.push_back(ShapePen(x, y));
        break;
    }
    case State::line:
    {
        shapes.push_back(ShapeLine(x, y));
        break;
    }
    case State::text:
    {
        shapes.push_back(ShapeText(x, y));
        break;
    }*/
    case State::mosaic:
    {
        //auto tool = ToolSub::Get();
        //if (tool->GetFill()) {
        //    shape = std::make_shared<ShapeMosaicRect>(x, y);
        //}
        //else {
        //    shape = std::make_shared<ShapeMosaic>(x, y);
        //}
        break;
    }
    case State::eraser:
    {
        //auto tool = ToolSub::Get();
        //if (tool->GetFill()) {
        //    shape = std::make_shared<ShapeEraserRect>(x, y);
        //}
        //else {
        //    shape = std::make_shared<ShapeEraser>(x, y);
        //}
        break;
    }
    default:
        break;
    }
}
