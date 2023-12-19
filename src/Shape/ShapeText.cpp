#include "ShapeText.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

static float fontSize{ 80 };

ShapeText::ShapeText(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeText::~ShapeText()
{
}

bool ShapeText::OnMouseDown(const int &x, const int &y)
{
    IsWIP = false;
    rect.setXYWH(x - 15, y - 50, 30, 100);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(2);
    canvas->drawRect(rect, paint);
    
    canvas->drawLine(startX, startY - 25, startX, startY + 25,paint);
    return false;
}

void ShapeText::initParams()
{
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 4;
        }
        else if (stroke == 2)
        {
            strokeWidth = 16;
        }
        else
        {
            strokeWidth = 28;
        }
    }
    color = tool->getColor();
}
