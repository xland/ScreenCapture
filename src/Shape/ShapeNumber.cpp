#include "ShapeNumber.h"
#include <numbers>
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"
static int num = 1;

ShapeNumber::ShapeNumber(const int &x, const int &y) : ShapeBase(x, y)
{
    number = num;
    num += 1;
    HoverIndex = 0;
    IsTemp = false;
    Draggers.push_back(SkRect::MakeEmpty());
    DraggerCursors.push_back(Cursor::cursor::all);
    DraggerCursors.push_back(Cursor::cursor::all);
    initParams();
}

ShapeNumber::~ShapeNumber()
{
    if (num > number) {
        num = number;
    }
}

bool ShapeNumber::OnMouseDown(const int &x, const int &y)
{
    hoverX = x;
    hoverY = y;
    if (path.isEmpty()) {
        endX = x;
        endY = y;
        makePath(startX, startY, x, y);
        auto win = App::GetWin();
        auto canvas = win->surfaceFront->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        Paint(canvas);
        App::GetWin()->Refresh();
    }
    return false;
}

bool ShapeNumber::OnMouseUp(const int &x, const int &y)
{
    IsWip = false;
    unsigned half = draggerSize / 2;
    Draggers[0].setXYWH(endX - half, endY - half, draggerSize, draggerSize);
    return false;
}

bool ShapeNumber::OnMouseMove(const int &x, const int &y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
    if (path.contains(x, y))
    {
        HoverIndex = 1;
        return true;
    }
    return false;
}

bool ShapeNumber::OnMoseDrag(const int &x, const int &y)
{
    if (HoverIndex == 0)
    {
        endX = x;
        endY = y;
        makePath(startX, startY, endX, endY);
    }
    else
    {
        auto xSpan = x - hoverX;
        auto ySpan = y - hoverY;
        startX += xSpan;
        startY += ySpan;
        endX += xSpan;
        endY += ySpan;
        makePath(startX, startY, endX, endY);
        hoverX = x;
        hoverY = y;
    }
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    App::GetWin()->Refresh();
    return false;
}

bool ShapeNumber::OnMouseWheel(const int& delta)
{
    auto span = 8.f;
    auto x = endX - startX;
    auto y = endY - startY;
    auto l = std::sqrt(x * x + y * y);//圆心到箭头顶点的长度    
    if (delta > 0) {
        endX += span * x / l;
        endY += span * y / l;
    }
    else {
        endX -= span * x / l;
        endY -= span * y / l;
    }
    makePath(startX, startY, endX, endY);
    auto win = App::GetWin();
    win->surfaceFront->getCanvas()->clear(SK_ColorTRANSPARENT);
    auto canvas = win->surfaceBack->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    unsigned half = draggerSize / 2;
    Draggers[0].setXYWH(endX - half, endY - half, draggerSize, draggerSize);
    App::GetWin()->Refresh();
    return false;
}

void ShapeNumber::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    if (stroke)
    {
        paint.setStroke(true);
        paint.setStrokeWidth(strokeWidth);
        paint.setStyle(SkPaint::Style::kStroke_Style);
    }
    paint.setColor(color);
    canvas->drawPath(path, paint);
    auto str = std::to_string(number);
    auto font = App::GetFontText();
    font->setSize(r);
    if (stroke) {
        paint.setStroke(false);
    }
    else {
        paint.setColor(SK_ColorWHITE);
    }    
    SkRect textBounds;
    font->measureText(str.c_str(), str.size(), SkTextEncoding::kUTF8, &textBounds);
    SkScalar x = startX - textBounds.width() / 2 - textBounds.left();
    SkScalar y = startY + textBounds.height() / 2 - textBounds.bottom();    
    canvas->drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, x, y, *font, paint);
}
void ShapeNumber::makePath(const int &x1, const int &y1, const int &x2, const int &y2)
{
    //x1,y1圆心；x2,y2箭头顶点
    auto x = x2 - x1;
    auto y = y1 - y2;
    r = std::sqrt(x * x + y * y);//圆心到箭头顶点的长度
    auto height = r / 3.0f; //箭头高度
    r = r - height; //半径
    auto radint = std::atan2(y, x); //反正切
    auto angle = radint * 180 / std::numbers::pi; //角度
    auto angleSpan = 16.f; //半角
    auto angle1 = (angle + angleSpan) * std::numbers::pi / 180;//弧度
    auto angle2 = (angle - angleSpan) * std::numbers::pi / 180;//弧度
    auto X1 = x1 + r * cos(angle1);//箭头与圆的交接点1
    auto Y1 = y1 - r * sin(angle1);
    auto X2 = x1 + r * cos(angle2);//箭头与圆的交接点2
    auto Y2 = y1 - r * sin(angle2);    
    path.reset(); 
    path.moveTo(X1, Y1);
    path.arcTo(r, r, 30, SkPath::kLarge_ArcSize, SkPathDirection::kCCW, X2, Y2);
    path.lineTo(x2, y2);
    path.close();
}
void ShapeNumber::initParams()
{
    auto tool = ToolSub::Get();
    stroke = !tool->GetFill();
    if (stroke)
    {
        auto stroke = tool->GetStroke();
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
    color = tool->GetColor();
}