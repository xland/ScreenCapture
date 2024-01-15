#include "ShapeEllipse.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"

ShapeEllipse::ShapeEllipse(const int &x, const int &y) : ShapeRect(x, y)
{
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

bool ShapeEllipse::OnMouseMove(const int& x, const int& y)
{
    bool flag = false;
    if (stroke) {
        auto halfStroke = strokeWidth / 2 + 2;
        auto rectOut = rect.makeOutset(halfStroke, halfStroke);
        auto rectInner = rect.makeInset(halfStroke, halfStroke);
        SkPath path;
        path.addOval(rectOut);
        path.addOval(rectInner);
        path.setFillType(SkPathFillType::kEvenOdd);
        flag = path.contains(x, y);
    }
    else {
        SkPath path;
        path.addOval(rect);
        flag = path.contains(x, y);
    }
    if (flag||MouseInDragger(x,y)) {
        return true;
    }
    return false;
}
