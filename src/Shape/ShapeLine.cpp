#include "ShapeLine.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapeLine::ShapeLine(const int &x, const int &y) : ShapeBase(x, y)
{
    path.moveTo(x, y);
    initParams();
}

ShapeLine::~ShapeLine()
{
}

bool ShapeLine::OnMouseDown(const int &x, const int &y)
{
    return false;
}

bool ShapeLine::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeLine::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeLine::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeLine::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    canvas->drawPath(path, paint);
    return false;
}

void ShapeLine::initParams()
{
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 4;
        }
        else if (stroke == 2)
        {
            strokeWidth = 16;
        }
        else
        {
            strokeWidth = 28;
        }
    }
    color = tool->getColor();
}
