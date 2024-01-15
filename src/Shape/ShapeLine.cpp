#include "ShapeLine.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"

ShapeLine::ShapeLine(const int &x, const int &y) : ShapeBase(x, y)
{
    for (size_t i = 0; i < 2; i++)
    {
        Draggers.push_back(SkRect::MakeEmpty());
        DraggerCursors.push_back(Cursor::cursor::all);
    }
    DraggerCursors.push_back(Cursor::cursor::all);
    initParams();
}

ShapeLine::~ShapeLine()
{
}

bool ShapeLine::OnMouseDown(const int &x, const int &y)
{
    hoverX = x;
    hoverY = y;
    return false;
}

bool ShapeLine::OnMouseMove(const int &x, const int &y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
    if (pathAssist.contains(x, y)) {
        HoverIndex = 2;
        return true;
    }
    return false;
}

bool ShapeLine::OnMouseUp(const int &x, const int &y)
{
    IsWip = false;

    auto p2 = path.getPoint(1);
    unsigned half = draggerSize / 2;
    Draggers[0].setXYWH(startX - half, startY - half, draggerSize, draggerSize);
    Draggers[1].setXYWH(p2.fX - half, p2.fY - half, draggerSize, draggerSize);

    pathAssist.reset();
    half = strokeWidth / 2 + 2;
    SkPoint p1(startX, startY);
    auto xSpan = std::abs(p2.fX - p1.fX);
    auto ySpan = std::abs(p2.fY - p1.fY);
    auto lineLength = std::sqrt(xSpan * xSpan + ySpan * ySpan);
    auto xPSpan = half * ySpan / lineLength;
    auto yPSpan = half * xSpan / lineLength;
    if ((p2.fX > p1.fX && p2.fY > p1.fY) || (p1.fX > p2.fX && p1.fY > p2.fY)) {
        pathAssist.moveTo(p1.fX + xPSpan, p1.fY - yPSpan);
        pathAssist.lineTo(p2.fX + xPSpan, p2.fY - yPSpan);
        pathAssist.lineTo(p2.fX - xPSpan, p2.fY + yPSpan);
        pathAssist.lineTo(p1.fX - xPSpan, p1.fY + yPSpan);
    }
    else {
        pathAssist.moveTo(p1.fX - yPSpan, p1.fY - xPSpan);
        pathAssist.lineTo(p2.fX - yPSpan, p2.fY - xPSpan);
        pathAssist.lineTo(p2.fX + yPSpan, p2.fY + xPSpan);
        pathAssist.lineTo(p1.fX + xPSpan, p1.fY + yPSpan);
    }
    pathAssist.close();

    return false;
}

bool ShapeLine::OnMoseDrag(const int &x, const int &y)
{
    IsTemp = false;
    auto p2 = path.getPoint(1);
    path.reset();
    if (HoverIndex == 0) {
        startX = x;
        startY = y;
        path.moveTo(startX, startY);
        path.lineTo(p2);
    }
    else if (HoverIndex == 1) {
        path.moveTo(startX, startY);
        path.lineTo(x, y);
    }
    else {
        auto xSpan = x - hoverX;
        auto ySpan = y - hoverY;
        startX += xSpan;
        startY += ySpan;
        p2.fX += xSpan;
        p2.fY += ySpan;
        path.moveTo(startX, startY);
        path.lineTo(p2);
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

void ShapeLine::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    if (fill) {
        paint.setColor(color);
    }
    else {
        auto c = SkColor4f::FromColor(color);
        c.fA = 0.3;
        paint.setColor4f(c);
    }
    
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    canvas->drawPath(path, paint);
}

void ShapeLine::initParams()
{
    HoverIndex = 1;
    auto tool = ToolSub::Get();
    fill = !tool->GetFill();
    auto stroke = tool->GetStroke();
    if (stroke == 1)
    {
        strokeWidth = 26;
    }
    else if (stroke == 2)
    {
        strokeWidth = 56;
    }
    else
    {
        strokeWidth = 86;
    }
    color = tool->GetColor();
}
