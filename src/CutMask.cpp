#include "CutMask.h"
#include "WindowMain.h"
#include "include/core/SkColor.h"
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

bool CutMask::OnMouseDown(const int& x, const int& y)
{    
    start.set(x, y);
    return false;
}
bool CutMask::OnMouseMove(const int& x, const int& y)
{
    return true;
}
bool CutMask::OnMouseDrag(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state != State::mask)
    {
        return false;
    }
    CutRect.setLTRB(start.x(), start.y(), x, y);
    CutRect.sort();
    path.reset();
    path.addRect(SkRect::MakeXYWH(0, 0, winMain->w, winMain->h));
    path.addRect(CutRect, SkPathDirection::kCW);
    path.setFillType(SkPathFillType::kEvenOdd);
    winMain->Refresh();
    return true;
}
bool CutMask::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setColor(SkColorSetARGB(160, 0, 0, 0));
    canvas->drawPath(path, paint);
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    paint.setStrokeWidth(3);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawRect(CutRect, paint);
    return false;
}
bool CutMask::OnMouseUp(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state != State::mask) {
        return false;
    }
    winMain->state = State::tool;
    winMain->Refresh();
    return true;
}