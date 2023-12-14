#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "WindowMain.h"

ToolMain *toolMain;

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
    if (!toolMain)
    {
        toolMain = new ToolMain();
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
    auto winMain = WindowMain::get();
    if (winMain && winMain->state >= State::tool) {
        auto mask = CutMask::get();
        SkRect rect;
        rect.setLTRB(mask->CutRect.right() - toolBoxWidth,mask->CutRect.bottom()+ toolBoxSpan, mask->CutRect.right(), mask->CutRect.bottom() + toolBoxSpan+ toolBoxHeight);
        SkPaint paint;
        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
        base->drawRect(rect, paint);
    }
    return false;
}

ToolMain::ToolMain()
{
}
