#include "ToolSub.h"

ToolSub *ToolSub;

ToolSub::~ToolSub()
{
}

void ToolSub::init()
{
    if (!ToolSub)
    {
        cutMask = new CutMask();
    }
}

ToolSub *ToolSub::get()
{
    return ToolSub;
}

bool ToolSub::OnMouseDown(int x, int y)
{
    return false;
}

bool ToolSub::OnMouseUp(int x, int y)
{
    return false;
}

bool ToolSub::OnMouseMove(int x, int y)
{
    return false;
}

bool ToolSub::OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas)
{
    return false;
}

bool ToolSub::OnPaintFinish(SkCanvas *base)
{
    return false;
}

ToolSub::ToolSub()
{
}
