#include "Rect.h"

Rect::Rect()
{
}

Rect::~Rect()
{
}

bool Rect::OnMouseDown(const int& x, const int& y)
{
    return false;
}

bool Rect::OnMouseUp(const int& x, const int& y)
{
    return false;
}

bool Rect::OnMouseMove(const int& x, const int& y)
{
    return false;
}

bool Rect::OnPaint(SkCanvas* canvas)
{
    return false;
}
