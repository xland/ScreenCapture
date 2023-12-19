#include "ShapePen.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapePen::ShapePen(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapePen::~ShapePen()
{
}

bool ShapePen::OnMouseDown(const int& x, const int& y)
{
    return false;
}

bool ShapePen::OnMouseMove(const int& x, const int& y)
{
    return false;
}

bool ShapePen::OnMouseUp(const int& x, const int& y)
{
    return false;
}

bool ShapePen::OnMoseDrag(const int& x, const int& y)
{
    IsWIP = false;
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapePen::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    canvas->drawPath(path,paint);
    return false;
}

void ShapePen::initParams()
{
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke) {
        auto stroke = tool->getStroke();
        if (stroke == 1) {
            strokeWidth = 4;
        }
        else if (stroke == 2) {
            strokeWidth = 16;
        }
        else
        {
            strokeWidth = 28;
        }
    }
    color = tool->getColor();
}
