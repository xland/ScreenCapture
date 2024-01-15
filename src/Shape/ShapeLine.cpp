#include "ShapeLine.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"

ShapeLine::ShapeLine(const int &x, const int &y) : ShapeBase(x, y)
{
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
    auto flag = pathAssist.contains(x, y);
    if (flag)
    {
        setDragger();
        return true;
    }
    return false;
}

bool ShapeLine::OnMouseUp(const int &x, const int &y)
{
    setDragger();
    auto win = App::GetWin();
    auto canvasBack = win->surfaceBack->getCanvas();
    Paint(canvasBack);
    IsWip = false;
    win->Refresh();

    pathAssist.reset();
    auto half = strokeWidth / 2 + 2;
    SkPoint p1(startX, startY);
    auto p2 = path.getPoint(1);
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

void ShapeLine::setDragger()
{
    //auto endPoint = path.getPoint(1);
    //auto shapeDragger = ShapeDragger::Get();
    //unsigned half = shapeDragger->Size / 2;
    //shapeDragger->SetDragger(0, startX - half, startY - half);
    //shapeDragger->SetDragger(1, endPoint.fX - half, endPoint.fY - half);
    //shapeDragger->DisableDragger(2);
    //shapeDragger->Cursors[0] = Cursor::cursor::all;
    //shapeDragger->Cursors[1] = Cursor::cursor::all;
    //shapeDragger->CurShape = this;
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
