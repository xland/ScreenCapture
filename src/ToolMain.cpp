#include "ToolMain.h"

ToolMain *toolMain;

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
    if (!toolMain)
    {
        cutMask = new CutMask();
    }
}

ToolMain *ToolMain::get()
{
    return toolMain;
}

bool ToolMain::OnMouseDown(int x, int y)
{
    return false;
}

bool ToolMain::OnMouseUp(int x, int y)
{
    return false;
}

bool ToolMain::OnMouseMove(int x, int y)
{
    return false;
}

bool ToolMain::OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas)
{
    return false;
}

bool ToolMain::OnPaintFinish(SkCanvas *base)
{
    return false;
}

ToolMain::ToolMain()
{
}
