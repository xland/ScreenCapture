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

bool ShapeEraserRect::OnMouseMove(const int& x, const int& y)
{
    if (rect.contains(x,y)) {
        setDragger();
        Cursor::All();
        HoverIndex = 8;
        ShapeDragger::Get()->ShowDragger(false);
        auto win = App::GetWin();
        auto canvas = win->surfaceFront->getCanvas();
        SkPaint paint;
        paint.setStroke(true);
        paint.setStrokeWidth(1);
        paint.setColor(SK_ColorBLACK);
        auto rectTemp = rect.makeOutset(2.f, 2.f);
        canvas->drawRect(rectTemp, paint);
        App::GetWin()->Refresh();
        return true;
    }
    return false;
}
