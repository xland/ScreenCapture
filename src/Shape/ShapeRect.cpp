#include "ShapeRect.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "../Timer.h"
#include "ShapeDragger.h"

ShapeRect::ShapeRect(const int& x, const int& y):ShapeBase(x,y), rect{SkRect::MakeXYWH(x,y,0,0)}
{
    initParams();
}

ShapeRect::~ShapeRect()
{
}

bool ShapeRect::OnMouseDown(const int& x, const int& y)
{
    startX = x;
    startY = y;
    return false;
}

bool ShapeRect::OnMouseUp(const int& x, const int& y)
{
    auto win = WindowMain::get();
    auto canvasBack = win->surfaceBack->getCanvas();
    Paint(canvasBack);
    isWip = false;
    setDragger();
    return false;
}

bool ShapeRect::OnMouseMove(const int& x, const int& y)
{
    auto flag = isMouseOver(x, y);
    if (flag) {
        setCursor();
        setDragger();
        Icon::myCursor(Icon::cursor::all);
        HoverIndex = 8;
        return true;
    }
    return false;
}

bool ShapeRect::OnMoseDrag(const int& x, const int& y)
{
    switch (HoverIndex)
    {
    case 0:
    {
        rect.fLeft = x;
        rect.fTop = y;
        break;
    }
    case 1:
    {
        rect.fTop = y;
        break;
    }
    case 2:
    {
        rect.fTop = y;
        rect.fRight = x;
        break;
    }
    case 3:
    {
        rect.fRight = x;
        break;
    }
    case 4:
    {
        rect.fRight = x;
        rect.fBottom = y;
        break;
    }
    case 5:
    {
        rect.fBottom = y;
        break;
    }
    case 6:
    {
        rect.fLeft = x;
        rect.fBottom = y;
        break;
    }
    case 7:
    {
        rect.fLeft = x;
        break;
    }
    case 8: {
        rect.offset(x - startX, y - startY);
        startX = x;
        startY = y;
        break;
    }
    default:
        break;
    }
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    WindowMain::get()->Refresh();
    return false;
}

void ShapeRect::setCursor()
{
    auto draggers = ShapeDragger::get();
    draggers->cursors[0] = Icon::cursor::wnse;
    draggers->cursors[4] = Icon::cursor::wnse;
    draggers->cursors[1] = Icon::cursor::ns;
    draggers->cursors[5] = Icon::cursor::ns;
    draggers->cursors[2] = Icon::cursor::nesw;
    draggers->cursors[6] = Icon::cursor::nesw;
    draggers->cursors[3] = Icon::cursor::we;
    draggers->cursors[7] = Icon::cursor::we;
}

void ShapeRect::setDragger()
{
    rect.sort();
    auto shapeDragger = ShapeDragger::get();
    unsigned half = shapeDragger->size/2;
    float l = rect.x() - half;
    float t = rect.y() - half;
    float r = rect.right() - half;
    float b = rect.bottom() - half;
    float wCenter = l + rect.width() / 2;
    float hCenter = t + rect.height() / 2;
    shapeDragger->setDragger(0, l, t);
    shapeDragger->setDragger(1, wCenter, t);
    shapeDragger->setDragger(2, r, t);
    shapeDragger->setDragger(3, r, hCenter);
    shapeDragger->setDragger(4, r, b);
    shapeDragger->setDragger(5, wCenter, b);
    shapeDragger->setDragger(6, l, b);
    shapeDragger->setDragger(7, l, hCenter);
}


bool ShapeRect::isMouseOver(const int& x, const int& y)
{
    if (stroke) {
        auto halfStroke = strokeWidth / 2 + 2;
        auto rectOut = rect.makeOutset(halfStroke, halfStroke);
        auto rectInner = rect.makeInset(halfStroke, halfStroke);
        if (rectOut.contains(x, y) && !rectInner.contains(x, y)) {
            return true;
        }
        return false;
    }
    else {
        return rect.contains(x, y);
    }
}

void ShapeRect::Paint(SkCanvas* canvas)
{
    SkPaint paint;
    if (stroke) {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);
    canvas->drawRect(rect, paint);
}


void ShapeRect::initParams()
{
    HoverIndex = 4;
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke) {
        auto stroke = tool->getStroke();
        if (stroke == 1) {
            strokeWidth = 4;
        }
        else if (stroke == 2) {
            strokeWidth = 8;
        }
        else
        {
            strokeWidth = 16;
        }
    }
    color = tool->getColor();
}
