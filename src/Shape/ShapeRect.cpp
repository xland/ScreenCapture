#include "ShapeRect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"

ShapeRect::ShapeRect(const int& x, const int& y):ShapeBase(x,y)
{
    rect = SkRect::MakeXYWH(x, y, 0, 0);
    HoverIndex = 4;
    for (size_t i = 0; i < 8; i++)
    {
        Draggers.push_back(SkRect::MakeEmpty());
    }
    DraggerCursors.push_back(Cursor::cursor::wnse);
    DraggerCursors.push_back(Cursor::cursor::ns);
    DraggerCursors.push_back(Cursor::cursor::nesw);
    DraggerCursors.push_back(Cursor::cursor::we);
    DraggerCursors.push_back(Cursor::cursor::wnse);
    DraggerCursors.push_back(Cursor::cursor::ns);
    DraggerCursors.push_back(Cursor::cursor::nesw);
    DraggerCursors.push_back(Cursor::cursor::we);
    DraggerCursors.push_back(Cursor::cursor::all);
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
    HoverIndex = -1;
    setDragger();
    return false;
}

bool ShapeRect::OnMouseMove(const int& x, const int& y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
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
        HoverIndex = 8;
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
    
    if (GetKeyState(VK_SHIFT) < 0) {
        SkScalar side = std::max(rect.width(), rect.height());
        rect.setXYWH(rect.fLeft, rect.fTop, side, side);
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
    unsigned half = draggerSize/2;
    float l = rect.x() - half;
    float t = rect.y() - half;
    float r = rect.right() - half;
    float b = rect.bottom() - half;
    float wCenter = l + rect.width() / 2;
    float hCenter = t + rect.height() / 2;
    Draggers[0].setXYWH(l, t,draggerSize,draggerSize);
    Draggers[1].setXYWH(wCenter, t, draggerSize, draggerSize);
    Draggers[2].setXYWH(r, t, draggerSize, draggerSize);
    Draggers[3].setXYWH(r, hCenter, draggerSize, draggerSize);
    Draggers[4].setXYWH(r, b, draggerSize, draggerSize);
    Draggers[5].setXYWH(wCenter, b, draggerSize, draggerSize);
    Draggers[6].setXYWH(l, b, draggerSize, draggerSize);
    Draggers[7].setXYWH(l, hCenter, draggerSize, draggerSize);
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
