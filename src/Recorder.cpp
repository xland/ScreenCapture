#include "Recorder.h"
#include "Shape/ShapeBase.h"
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

bool Recorder::OnMouseDown(const int& x, const int& y)
{
    for (auto& shape:shapes)
    {
        shape->OnMouseDown(x, y);
    }
    return false;
}

bool Recorder::OnMouseUp(const int& x, const int& y)
{
    for (auto& shape : shapes)
    {
        shape->OnMouseUp(x, y);
    }
    return false;
}

bool Recorder::OnMouseMove(const int& x, const int& y)
{
    for (auto& shape : shapes)
    {
        shape->OnMouseMove(x, y);
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
