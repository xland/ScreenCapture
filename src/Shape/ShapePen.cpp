#include "ShapePen.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "ShapeDragger.h"

ShapePen::ShapePen(const int &x, const int &y) : ShapeBase(x, y)
{
    isWip = false;
    ShapeDragger::get()->disableDragger();
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
    path.lineTo(x, y);
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    WindowMain::get()->Refresh();
    return false;
}

void ShapePen::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);    
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    canvas->drawPath(path,paint);
}

void ShapePen::initParams()
{
    path.moveTo(startX, startY);
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
