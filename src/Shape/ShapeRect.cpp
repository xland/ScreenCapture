#include "ShapeRect.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

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
    rect.sort();
    unsigned size = 10;
    unsigned half = 5;
    float l = rect.x() - half;
    float t = rect.y() - half;
    float r = rect.right() - half;
    float b = rect.bottom() - half;
    float wCenter = l + rect.width() / 2;
    float hCenter = t + rect.height() / 2;
    draggers[0].setXYWH(l, t,size,size);
    draggers[1].setXYWH(wCenter, t, size, size); 
    draggers[2].setXYWH(r, t, size, size);
    draggers[3].setXYWH(r, hCenter, size, size);
    draggers[4].setXYWH(r, b, size, size);
    draggers[5].setXYWH(wCenter, b, size, size);
    draggers[6].setXYWH(l, b, size, size);
    draggers[7].setXYWH(l, hCenter, size, size);
    return false;
}

bool ShapeRect::OnMouseMove(const int& x, const int& y)
{
    for (size_t i = 0; i < draggers.size(); i++)
    {
        if (draggers[i].contains(x, y)) {
            HoverIndex = i;
            setCursor();
            if (!showDragger) {
                showDragger = true;
                WindowMain::get()->Refresh();
            }
            return true;
        }
    }
    auto flag = isMouseOver(x, y);
    if (flag) {
        HoverIndex = 8;
        setCursor();
        if (!showDragger) {
            showDragger = true;
            WindowMain::get()->Refresh();
        }
        return true;
    }
    return false;
}

bool ShapeRect::OnPaint(SkCanvas* canvas)
{
    SkPaint paint;
    if (stroke) {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);    
    canvas->drawRect(rect, paint);
    paintDragger(canvas);
    return false;
}

bool ShapeRect::OnMoseDrag(const int& x, const int& y)
{
    IsWIP = false;
    showDragger = false;
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

        rect.offset(x-startX, y-startY);
        startX = x;
        startY = y;
        break;
    }
    default:
        break;
    }
    WindowMain::get()->Refresh();
    return false;
}

void ShapeRect::setCursor()
{
    switch (HoverIndex)
    {
    case 0:
    case 4:
    {
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
        break;
    }
    case 1:
    case 5:
    {
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
        break;
    }
    case 2:
    case 6:
    {
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
        break;
    }
    case 3:
    case 7:
    {
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
        break;
    }
    case 8: {
        SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
        break;
    }
    default:
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        break;
    }
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

void ShapeRect::paintDragger(SkCanvas* canvas)
{
    if (!showDragger) {
        return;
    }
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : draggers) {
        canvas->drawRect(dragger, paint);
    }
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
    for (size_t i = 0; i < 8; i++)
    {
        draggers.push_back(SkRect::MakeEmpty());
    }    
}
