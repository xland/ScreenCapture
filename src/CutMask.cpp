#include "CutMask.h"
#include "WindowMain.h"
#include "include/core/SkColor.h"
#include "State.h"

CutMask::CutMask()
{
    for (size_t i = 0; i < 8; i++)
    {
        masks.push_back(SkRect());
    }
}

CutMask::~CutMask()
{
}

bool CutMask::OnMouseDown(int x, int y)
{    
    if (state < State::mask) {
        start.set(x, y);
        state = State::mask;
    }    
    return false;
}
bool CutMask::OnMouseMove(int x, int y)
{
    auto winMain = WindowMain::get();
    if (!winMain->IsMouseDown || state != State::mask)
    {
        return false;
    }
    rectCenter.setLTRB(start.x(), start.y(), x, y);
    rectCenter.sort();
    masks[0].setLTRB(0, 0, rectCenter.left(), rectCenter.top());
    masks[1].setLTRB(rectCenter.left(), 0, rectCenter.right(), rectCenter.top());
    masks[2].setLTRB(rectCenter.right(), 0, winMain->w, rectCenter.top());
    masks[3].setLTRB(rectCenter.right(), rectCenter.top(), winMain->w, rectCenter.bottom());
    masks[4].setLTRB(rectCenter.right(), rectCenter.bottom(), winMain->w, winMain->h);
    masks[5].setLTRB(rectCenter.left(), rectCenter.bottom(), rectCenter.right(), winMain->h);
    masks[6].setLTRB(0, rectCenter.bottom(), rectCenter.left(), winMain->h);
    masks[7].setLTRB(0, rectCenter.top(), rectCenter.left(), rectCenter.bottom());
    winMain->Refresh();
    return false;
}
bool CutMask::OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas)
{
    if (state == State::mask)
    {
        SkPaint paint;
        paint.setColor(SkColorSetARGB(160, 0, 0, 0));
        canvas->clear(SK_ColorTRANSPARENT);
        for (size_t i = 0; i < 8; i++)
        {
            canvas->drawRect(masks[i], paint);
        }
    }
    return false;
}
bool CutMask::OnPaintFinish(SkCanvas *base)
{
    if (state <= State::mask)
    {
        return false;
    }
    SkPaint paint;
    paint.setColor(SkColorSetARGB(160, 0, 0, 0));
    for (size_t i = 0; i < 8; i++)
    {
        base->drawRect(masks[i], paint);
    }
    return false;
}
bool CutMask::OnMouseUp(int x, int y)
{
    state = State::tool;
    return false;
}