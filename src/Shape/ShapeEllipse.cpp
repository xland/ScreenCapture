#include "ShapeEllipse.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapeEllipse::ShapeEllipse(const int &x, const int &y) : ShapeRect(x, y)
{
    initParams();
}

ShapeEllipse::~ShapeEllipse()
{
}

bool ShapeEllipse::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    if (stroke)
    {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);
    canvas->drawOval(rect, paint);
    paintDragger(canvas);
    return false;
}

bool ShapeEllipse::isMouseOnBorder(const int& x, const int& y)
{
    auto halfStroke = strokeWidth / 2 + 2;
    auto rectOut = rect.makeOutset(halfStroke, halfStroke);
    auto rectInner = rect.makeInset(halfStroke, halfStroke);
    SkPath path;
    path.addOval(rectOut);
    path.addOval(rectInner);
    path.setFillType(SkPathFillType::kEvenOdd);
    return path.contains(x, y);
}
