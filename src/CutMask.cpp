#include "CutMask.h"
#include "WindowMain.h"
#include "include/core/SkColor.h"
#include "State.h"

CutMask* cutMask;

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
    masks[0].setLTRB(0, 0, CutRect.left(), CutRect.top());
    masks[1].setLTRB(CutRect.left(), 0, CutRect.right(), CutRect.top());
    masks[2].setLTRB(CutRect.right(), 0, winMain->w, CutRect.top());
    masks[3].setLTRB(CutRect.right(), CutRect.top(), winMain->w, CutRect.bottom());
    masks[4].setLTRB(CutRect.right(), CutRect.bottom(), winMain->w, winMain->h);
    masks[5].setLTRB(CutRect.left(), CutRect.bottom(), CutRect.right(), winMain->h);
    masks[6].setLTRB(0, CutRect.bottom(), CutRect.left(), winMain->h);
    masks[7].setLTRB(0, CutRect.top(), CutRect.left(), CutRect.bottom());
    winMain->Refresh();
    return false;
}
bool CutMask::OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (winMain && winMain->state == State::mask)
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
    auto winMain = WindowMain::get();
    if (winMain && winMain->state <= State::mask)
    {
        return false;
    }
    SkPaint paint;
    paint.setColor(SkColorSetARGB(160, 0, 0, 0));
    for (size_t i = 0; i < 8; i++)
    {
        base->drawRect(masks[i], paint);
    }
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    paint.setStrokeWidth(4);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    base->drawRect(CutRect, paint);
    return false;
}
bool CutMask::OnMouseUp(int x, int y)
{
    auto winMain = WindowMain::get();
    winMain->state = State::tool;
    winMain->surfaceCanvas->getCanvas()->clear(SK_ColorTRANSPARENT);
    winMain->Refresh();
    return false;
}