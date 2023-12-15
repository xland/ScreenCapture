#include "CutMask.h"
#include "WindowMain.h"
#include "include/core/SkColor.h"
#include "include/core/SkPath.h"
#include "State.h"

CutMask* cutMask;

CutMask::CutMask()
{
}

CutMask::~CutMask()
{
}

void CutMask::init()
{
    if (!cutMask)
    {
        cutMask = new CutMask();
    }
}

CutMask* CutMask::get()
{
    return cutMask;
}

bool CutMask::OnMouseDown(int x, int y)
{    
    auto winMain = WindowMain::get();
    if (winMain->state < State::mask) {
        start.set(x, y);
        winMain->state = State::mask;
    }
    return false;
}
bool CutMask::OnMouseMove(int x, int y)
{
    auto winMain = WindowMain::get();
    if (!winMain->IsMouseDown || winMain->state != State::mask)
    {
        return false;
    }
    CutRect.setLTRB(start.x(), start.y(), x, y);
    CutRect.sort();
    winMain->Refresh();
    return false;
}
bool CutMask::OnPaint(SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (winMain && winMain->state == State::mask)
    {
        canvas->clear(SK_ColorTRANSPARENT);
        SkPaint paint;
        paint.setColor(SkColorSetARGB(160, 0, 0, 0));        
        SkPath path;
        path.addRect(SkRect::MakeXYWH(0, 0, winMain->w, winMain->h));
        path.addRect(CutRect, SkPathDirection::kCW);
        path.setFillType(SkPathFillType::kEvenOdd);
        canvas->drawPath(path, paint);
        paint.setColor(SkColorSetARGB(255, 22, 118, 255));
        paint.setStrokeWidth(4);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        canvas->drawRect(CutRect, paint);
    }
    return false;
}
bool CutMask::OnMouseUp(int x, int y)
{
    auto winMain = WindowMain::get();
    winMain->state = State::tool;
    winMain->Refresh();
    return false;
}