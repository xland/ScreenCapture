#include "ShapeRect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Timer.h"
#include "../Cursor.h"
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
    IsWip = false;
    setDragger();
    return false;
}

bool ShapeRect::OnMouseMove(const int& x, const int& y)
{
    bool flag = false;
    if (stroke) {
        auto halfStroke = strokeWidth / 2 + 2;
        auto rectOut = rect.makeOutset(halfStroke, halfStroke);
        auto rectInner = rect.makeInset(halfStroke, halfStroke);
        if (rectOut.contains(x, y) && !rectInner.contains(x, y)) {
            flag = true;
        }
        else
        {
            flag = false;
        }
    }
    else {
        flag = rect.contains(x, y);
    }
    if (flag) {
        setDragger();
        Cursor::All();
        HoverIndex = 8;
        App::GetWin()->Refresh();
        return true;
    }
    return false;
}

bool ShapeRect::OnMoseDrag(const int& x, const int& y)
{
    IsTemp = false;
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
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    App::GetWin()->Refresh();
    return false;
}

void ShapeRect::setDragger()
{
    rect.sort();
    auto shapeDragger = ShapeDragger::Get();
    unsigned half = shapeDragger->Size/2;
    float l = rect.x() - half;
    float t = rect.y() - half;
    float r = rect.right() - half;
    float b = rect.bottom() - half;
    float wCenter = l + rect.width() / 2;
    float hCenter = t + rect.height() / 2;
    shapeDragger->SetDragger(0, l, t);
    shapeDragger->SetDragger(1, wCenter, t);
    shapeDragger->SetDragger(2, r, t);
    shapeDragger->SetDragger(3, r, hCenter);
    shapeDragger->SetDragger(4, r, b);
    shapeDragger->SetDragger(5, wCenter, b);
    shapeDragger->SetDragger(6, l, b);
    shapeDragger->SetDragger(7, l, hCenter);
    shapeDragger->Cursors[0] = Cursor::cursor::wnse;
    shapeDragger->Cursors[4] = Cursor::cursor::wnse;
    shapeDragger->Cursors[1] = Cursor::cursor::ns;
    shapeDragger->Cursors[5] = Cursor::cursor::ns;
    shapeDragger->Cursors[2] = Cursor::cursor::nesw;
    shapeDragger->Cursors[6] = Cursor::cursor::nesw;
    shapeDragger->Cursors[3] = Cursor::cursor::we;
    shapeDragger->Cursors[7] = Cursor::cursor::we;
    shapeDragger->CurShape = this;
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
    auto tool = ToolSub::Get();
    stroke = !tool->GetFill();
    if (stroke) {
        auto stroke = tool->GetStroke();
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
    color = tool->GetColor();
}
