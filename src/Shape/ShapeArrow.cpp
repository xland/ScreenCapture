#include "ShapeArrow.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

ShapeArrow::ShapeArrow(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeArrow::~ShapeArrow()
{
}

bool ShapeArrow::OnMouseDown(const int &x, const int &y)
{
    startX = x;
    startY = y;
    return false;
}

bool ShapeArrow::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeArrow::OnMouseMove(const int &x, const int &y)
{
    for (size_t i = 0; i < draggers.size(); i++)
    {
        if (draggers[i].contains(x, y))
        {
            HoverIndex = i;
            setCursor();
            if (!showDragger)
            {
                showDragger = true;
                WindowMain::get()->Refresh();
            }
            return true;
        }
    }
    auto flag = isMouseOnBorder(x, y);
    if (flag)
    {
        HoverIndex = 8;
        setCursor();
        if (!showDragger)
        {
            showDragger = true;
            WindowMain::get()->Refresh();
        }
        return true;
    }
    return false;
}

bool ShapeArrow::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    showDragger = false;
    path.reset();
    path.moveTo(startX, startY);
    //path.lineTo(x2, y2);
    double height = 32.0;
    double width = 32.0;
    auto x = x - startX;
    auto y = startY - y;
    auto z = sqrt(x * x + y * y);
    auto sin = y / z;
    auto cos = x / z;
    // △底边的中点
    double centerX = x - height * cos;
    double centerY = y + height * sin;
    double tempA = width / 4 * sin;
    double tempB = width / 4 * cos;
    // △ 左下的顶点与底边中点之间中间位置的点
    double X1 = centerX - tempA;
    double Y1 = centerY - tempB;
    path.lineTo(X1, Y1);
    // △ 左下的顶点
    double X2 = X1 - tempA;
    double Y2 = Y1 - tempB;
    path.lineTo(X2, Y2);
    // △ 上部顶点，也就是箭头终点
    path.lineTo(x, y);
    // △ 右下顶点
    tempA = width / 2 * sin;
    tempB = width / 2 * cos;
    double X3 = centerX + tempA;
    double Y3 = centerY + tempB;
    path.lineTo(X3, Y3);
    // △ 右下的顶点与底边中点之间中间位置的点
    double X4 = centerX + tempA / 2;
    double Y4 = centerY + tempB / 2;
    path.lineTo(X4, Y4);
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeArrow::OnPaint(SkCanvas* canvas)
{
    SkPaint paint;
    if (stroke)
    {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
    }
    paint.setColor(color);
    canvas->drawPath(path, paint);
    //paintDragger(canvas);
    return false;
}

void ShapeArrow::setCursor()
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
    case 8:
    {
        SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
        break;
    }
    default:
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        break;
    }
}

bool ShapeArrow::isMouseOnBorder(const int &x, const int &y)
{
    return false;
}

void ShapeArrow::paintDragger(SkCanvas *canvas)
{
    if (!showDragger)
    {
        return;
    }
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto &dragger : draggers)
    {
        canvas->drawRect(dragger, paint);
    }
}

void ShapeArrow::initParams()
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
