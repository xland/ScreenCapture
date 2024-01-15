#include "ShapeEraserRect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"

ShapeEraserRect::ShapeEraserRect(const int &x, const int &y) : ShapeRect(x, y)
{
}

ShapeEraserRect::~ShapeEraserRect()
{
}
void ShapeEraserRect::Paint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    auto backCanvas = win->surfaceBack->getCanvas();
    SkPaint paint;
    paint.setColor(color);
    paint.setBlendMode(SkBlendMode::kClear);
    backCanvas->drawRect(rect, paint);
}
void ShapeEraserRect::OnShowDragger(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    canvas->drawRect(rect, paint);
}
bool ShapeEraserRect::OnMouseMove(const int& x, const int& y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
    if (rect.contains(x,y)) {
        HoverIndex = 8;
        return true;
    }
    return false;
}
