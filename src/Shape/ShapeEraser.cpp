#include "ShapeEraser.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "ShapeDragger.h"

ShapeEraser::ShapeEraser(const int &x, const int &y) : ShapeBase(x, y)
{
    isWip = false;
    initParams();
}

ShapeEraser::~ShapeEraser()
{
}

void ShapeEraser::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setBlendMode(SkBlendMode::kClear);
    canvas->drawPath(path, paint);
}

bool ShapeEraser::OnMouseDown(const int& x, const int& y)
{
    return false;
}

bool ShapeEraser::OnMouseUp(const int& x, const int& y)
{
    ShapeDragger::get()->disableDragger();    
    return false;
}

bool ShapeEraser::OnMouseMove(const int& x, const int& y)
{
    return false;
}

bool ShapeEraser::OnMoseDrag(const int& x, const int& y)
{
    path.lineTo(x, y);
    auto win = WindowMain::get();
    auto canvas = win->surfaceBack->getCanvas();
    Paint(canvas);
    WindowMain::get()->Refresh();
    return false;
}

void ShapeEraser::initParams()
{
    HoverIndex = 4;
    path.moveTo(startX, startY);
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke) {
        auto stroke = tool->getStroke();
        if (stroke == 1) {
            strokeWidth = 26;
        }
        else if (stroke == 2) {
            strokeWidth = 56;
        }
        else
        {
            strokeWidth = 86;
        }
    }
}