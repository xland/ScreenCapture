#include "Recorder.h"
#include "Shape/ShapeBase.h"
#include "WindowMain.h"
#include "Shape/Rect.h"
Recorder *recorder;

Recorder::Recorder():wipIndex{-1}
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

bool Recorder::OnMouseDown(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state < State::rect) {
        wipIndex = -1;
        return false;
    }
    bool flag = false;
    for (int i=shapes.size()-1;i>=0;i--)
    {
        flag = shapes[i]->OnMouseDown(x, y);
        if (flag) {
            wipIndex = i;
            return false;
        }
    }
    switch (winMain->state)
    {
    case State::rect: {
        shapes.push_back(std::make_shared<Rect>());
        wipIndex = shapes.size()-1;
        break;
    }
    case State::ellipse: {
        break;
    }
    case State::arrow: {
        break;
    }
    case State::number: {
        break;
    }
    case State::pen: {
        break;
    }
    case State::line: {
        break;
    }
    case State::text: {
        break;
    }
    case State::mosaic: {
        break;
    }
    case State::eraser: {
        break;
    }
    default:
        return false;
        break;
    }
    shapes[wipIndex]->OnMouseDown(x, y);
    return false;
}
bool Recorder::OnMouseMove(const int& x, const int& y)
{  
    if (wipIndex < 0) {
        return false;
    }
    for (auto& shape : shapes)
    {
        shape->OnMouseMove(x, y);
    }
    return false;
}
bool Recorder::OnMouseUp(const int& x, const int& y)
{
    if (wipIndex < 0) {
        return false;
    }
    else {
        if (shapes[wipIndex]->IsWIP) {
            shapes.erase(shapes.begin() + wipIndex, shapes.begin() + 1);
        }
        wipIndex = -1;
        return false;
    }
    wipIndex = -1;
    for (auto& shape : shapes)
    {
        shape->OnMouseUp(x, y);
    }
    return false;
}



bool Recorder::OnPaint(SkCanvas* canvas)
{
    for (auto& shape : shapes)
    {
        shape->OnPaint(canvas);
    }
    return false;
}
