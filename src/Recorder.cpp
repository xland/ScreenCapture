#include "Recorder.h"

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

bool Recorder::OnMouseDown(int x, int y)
{
    return false;
}

bool Recorder::OnMouseUp(int x, int y)
{
    return false;
}

bool Recorder::OnMouseMove(int x, int y)
{
    return false;
}

bool Recorder::OnPaint(SkCanvas* canvas)
{
    return false;
}
