#include "pch.h"
#include "WinToolSubSlider.h"
#include "Shape/ShapeTextWin.h"
#include "WinToolSub.h"

WinToolSubSlider::WinToolSubSlider(WinToolSub* win):win{win}
{
}

WinToolSubSlider::~WinToolSubSlider()
{
}

void WinToolSubSlider::paint()
{
	if (!visible) return;
	auto y{ win->marginTop + win->btnSize / 2 };
	win->render->DrawLine({ start,y }, { start + 120.f, y }, win->brushSpliter.Get(), win->dpi);
	auto r{ 4.f * win->dpi };
	auto x{ start + (end - start) * ((val - min) / (max - min)) };
	win->render->FillEllipse({ {x,y}, r, r }, win->brushSpliter.Get());
}
void WinToolSubSlider::mouseWheel(const int& x, const int& y, const short& delta)
{
    if (!visible || x > end || x < start) return;
    if (delta < 0) {
        if (val < min) {
            val = min;
        }
        else if (val > min) {
            val -= 1.f;
        }
        else {
            return;
        }
    }
    else
    {
        if (val > max) {
            val = max;
        }
        else if (val < max) {
            val += 1.f;
        }
        else {
            return;
        }
    }
    win->refresh();
}
void WinToolSubSlider::mouseDrag(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return;
    val = ((float)x - start) / (end - start) * (max - min) + min;
    win->tipText = std::format(L"{}", std::round(val));
    win->showTipAt(win->x + x, int(win->y + win->marginTop + 4 * win->dpi + 0.5));
    win->refresh();
}
void WinToolSubSlider::mouseMove(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return;
    float val = ((float)x - start) / (end - start) * (max - min) + min;
    win->tipText = std::format(L"{}", std::round(val));;
    win->showTipAt(win->x + x, int(win->y + win->marginTop + 4 * win->dpi + 0.5));
}

bool WinToolSubSlider::mouseDown(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return false;
    val = ((float)x - start) / (end - start) * (max - min) + min;
    ShapeTextWin::get()->changeState();
    return true;
}

void WinToolSubSlider::setVals(const float& max, const float& min, const float& val, const float& start, bool visible)
{
    this->max = max;
    this->min = min;
    this->val = val;
    this->start = start;
    this->visible = visible;
    this->end = start + 80 * win->dpi;
}
