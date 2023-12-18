#include "Rect.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

Rect::Rect():rect{SkRect::MakeEmpty()}
{
    initParams();
}

Rect::~Rect()
{
}

bool Rect::OnMouseDown(const int& x, const int& y)
{
    startX = x;
    startY = y;
    return false;
}

bool Rect::OnMouseUp(const int& x, const int& y)
{
    draggers.push_back(SkRect::MakeXYWH(rect.x() - 6, rect.y() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.x()+ rect.width()/2 - 6, rect.y() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.right() - 6, rect.y() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.right() - 6, rect.y()+rect.height()/2 - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.right() - 6, rect.y() + rect.height() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.x() + rect.width() / 2 - 6, rect.bottom() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.x()- 6, rect.bottom() - 6, 12, 12));
    draggers.push_back(SkRect::MakeXYWH(rect.x() - 6, rect.y() + rect.height() / 2 - 6, 12, 12));
    WindowMain::get()->Refresh();
    return false;
}

bool Rect::OnMouseMove(const int& x, const int& y)
{
    if (IsWIP) {
        IsWIP = false;
    }
    rect.setLTRB(startX, startY, x, y);
    WindowMain::get()->Refresh();
    return true;
}

bool Rect::OnPaint(SkCanvas* canvas)
{
    SkPaint paint;
    if (stroke) {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);    
    canvas->drawRect(rect, paint);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : draggers) {
        canvas->drawRect(dragger, paint);
    }
    return false;
}

bool Rect::OnCheckHover(const int& x, const int& y)
{
    auto halfStroke = strokeWidth / 2+2;
    auto rectOut = rect.makeOutset(halfStroke,halfStroke);
    auto rectInner = rect.makeInset(halfStroke, halfStroke);
    if (rectOut.contains(x, y) && !rectInner.contains(x, y)) {
        IsHover = true;
        return true;
    }
    return false;
}

void Rect::initParams()
{
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
