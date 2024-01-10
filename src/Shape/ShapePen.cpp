#include "ShapePen.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "ShapeDragger.h"

ShapePen::ShapePen(const int &x, const int &y) : ShapeBase(x, y)
{
    IsWip = false;
    ShapeDragger::Get()->DisableDragger();
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
    IsTemp = false;
    path.lineTo(x, y);
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    App::GetWin()->Refresh();
    return false;
}

void ShapePen::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);    
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
    auto tool = ToolSub::Get();
    stroke = true;
    auto strokeLine = tool->GetStroke();
    if (strokeLine == 1) {
        strokeWidth = 4;
    }
    else if (strokeLine == 2) {
        strokeWidth = 16;
    }
    else
    {
        strokeWidth = 28;
    }
    color = tool->GetColor();
}
