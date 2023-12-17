#include "Rect.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

Rect::Rect()
{
}

Rect::~Rect()
{
}

bool Rect::OnMouseDown(const int& x, const int& y)
{
    startX = x;
    startY = y;
    return false;
}

bool Rect::OnMouseUp(const int& x, const int& y)
{
    return false;
}

bool Rect::OnMouseMove(const int& x, const int& y)
{
    rect.setLTRB(startX, startY, x, y);
    WindowMain::get()->Refresh();
    IsWIP = false;
    return false;
}

bool Rect::OnPaint(SkCanvas* canvas)
{
    SkPaint paint;
    auto tool = ToolSub::get();
    if (tool->getFill()) {
        paint.setStroke(false);
    }
    else
    {
        paint.setStroke(true);
        auto stroke = tool->getStroke();
        if (stroke == 1) {
            paint.setStrokeWidth(4);
        }
        else if (stroke == 2) {
            paint.setStrokeWidth(8);
        }
        else
        {
            paint.setStrokeWidth(16);
        }
    }
    paint.setColor(tool->getColor());    
    canvas->drawRect(rect, paint);
    return false;
}
