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

void ShapeEllipse::Paint(SkCanvas *canvas)
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
}

bool ShapeEllipse::isMouseOver(const int& x, const int& y)
{
    if (stroke) {
        auto halfStroke = strokeWidth / 2 + 2;
        auto rectOut = rect.makeOutset(halfStroke, halfStroke);
        auto rectInner = rect.makeInset(halfStroke, halfStroke);
        SkPath path;
        path.addOval(rectOut);
        path.addOval(rectInner);
        path.setFillType(SkPathFillType::kEvenOdd);
        return path.contains(x, y);
    }
    else {
        SkPath path;
        path.addOval(rect);
        return path.contains(x, y);
    }
}
