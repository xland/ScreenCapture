#include "ToolSub.h"
#include "State.h"
#include "WindowMain.h"
#include "ToolMain.h"

ToolSub *toolSub;

ToolSub::ToolSub()
{
}

ToolSub::~ToolSub()
{
}

void ToolSub::init()
{
    if (!toolSub)
    {
        toolSub = new ToolSub();
    }
}

ToolSub *ToolSub::get()
{
    return toolSub;
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

bool ToolSub::OnPaint(SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    auto toolMain = ToolMain::get();
    if (toolMain->indexSelected < 0)
    {
        return false;
    }
    auto left = toolMain->toolRect.left();
    auto top = toolMain->toolRect.bottom() + toolMain->marginTop;
    toolRect.setXYWH(left, top, 12 * toolMain->btnWidth, toolMain->height);
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawRoundRect(toolRect, 3, 3, paint);
    return false;
}
