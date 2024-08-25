#include "CutMask.h"
#include "WinMax.h"


CutMask::CutMask()
{
}

CutMask::~CutMask()
{
}

void CutMask::Paint(SkCanvas* canvas)
{
}

bool CutMask::onLeftBtnDown(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state == State::start) {
        cutRect.setLTRB(x, y, x, y);
        hoverIndex = 4;
        win->state = State::mask;
        win->canvas->clear(SK_ColorTRANSPARENT);
        return true;
    }
    if (hoverIndex == 0) {
        cutRect.setLTRB(x, y, cutRect.fRight, cutRect.fBottom);
        start.set(cutRect.fRight, cutRect.fBottom);
    }
    else if (hoverIndex == 1) {
        cutRect.setLTRB(cutRect.fLeft, y, cutRect.fRight, cutRect.fBottom);
        start.set(0, cutRect.fBottom);
    }
    else if (hoverIndex == 2) {
        cutRect.setLTRB(cutRect.fLeft, y, x, cutRect.fBottom);
        start.set(cutRect.fLeft, cutRect.fBottom);
    }
    else if (hoverIndex == 3) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, x, cutRect.fBottom);
        start.set(cutRect.fLeft, 0);
    }
    else if (hoverIndex == 4) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, x, y);
        start.set(cutRect.fLeft, cutRect.fTop);
    }
    else if (hoverIndex == 5) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, cutRect.fRight, y);
        start.set(0, cutRect.fTop);
    }
    else if (hoverIndex == 6) {
        cutRect.setLTRB(x, cutRect.fTop, cutRect.fRight, y);
        start.set(cutRect.fRight, cutRect.fTop);
    }
    else if (hoverIndex == 7) {
        cutRect.setLTRB(x, cutRect.fTop, cutRect.fRight, cutRect.fBottom);
        start.set(cutRect.fRight, 0);
    }
    else if (hoverIndex == 8) {
        start.set(x - cutRect.fLeft, y - cutRect.fTop);
        return true;
    }
    else {
        return false;
    }
    win->state = State::mask;
    win->canvas->clear(SK_ColorTRANSPARENT);
    return true;
}

bool CutMask::onLeftBtnUp(const int& x, const int& y)
{
}

bool CutMask::onMouseMove(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state < State::mask)
    {
        for (size_t i = 0; i < winRects.size(); i++)
        {
            if (winRects[i].contains(x, y)) {
                cutRect = winRects[i];
                return false;
            }
        }
        return false;
    }
}

bool CutMask::onMouseDrag(const int& x, const int& y)
{
    return false;
}
