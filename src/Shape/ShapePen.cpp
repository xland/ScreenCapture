#include "ShapePen.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapePen::ShapePen(const int &x, const int &y) : ShapeBase(x, y)
{
}

ShapePen::~ShapePen()
{
}

bool ShapePen::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    return false;
}
