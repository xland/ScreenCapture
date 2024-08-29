#include "ShapeEraserRect.h"
#include "include/core/SkPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"
#include "../Recorder.h"

ShapeEraserRect::ShapeEraserRect(const int &x, const int &y) : ShapeRect(x, y)
{
}

ShapeEraserRect::~ShapeEraserRect()
{
}
void ShapeEraserRect::Paint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    if (win->IsMouseDown && Recorder::Get()->CurShape == this) {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStroke(true);
        paint.setStrokeWidth(1);
        SkScalar intvls[] = { 6, 6 };
        auto effect = SkDashPathEffect::Make(intvls, 2, 0);
        paint.setPathEffect(effect);
        paint.setColor(SK_ColorBLACK);
        canvas->drawRect(rect, paint);
    }
    auto backCanvas = win->surfaceBack->getCanvas();
    SkPaint paint;
    paint.setColor(color);
    paint.setBlendMode(SkBlendMode::kClear);
    backCanvas->drawRect(rect, paint);
}
void ShapeEraserRect::OnShowDragger(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    SkScalar intvls[] = { 6, 6 };
    auto effect = SkDashPathEffect::Make(intvls, 2, 0);
    paint.setPathEffect(effect);
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
