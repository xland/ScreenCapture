#include "pch.h"
#include "ToolSubSlider.h"
#include "Shape/ShapeTextWin.h"
#include "ToolSub.h"

ToolSubSlider::ToolSubSlider(ToolSub* win):win{win}
{
}

ToolSubSlider::~ToolSubSlider()
{
}

void ToolSubSlider::paint()
{
	if (!visible) return;
	auto y{ win->marginTop + win->btnSize / 2 };
	win->render->DrawLine({ start,y }, { end, y }, win->brushSpliter.Get(), win->dpi);
	auto r{ 4.f * win->dpi };
	auto x{ start + (end - start) * ((val - min) / (max - min)) };
	win->render->FillEllipse({ {x,y}, r, r }, win->brushSpliter.Get());
}
float ToolSubSlider::getRequiredWidth()
{
	return visible ? end + 4.f * win->dpi : 0;
}

void ToolSubSlider::mouseWheel(const int& x, const int& y, const short& delta)
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
void ToolSubSlider::mouseDrag(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return;
    val = ((float)x - start) / (end - start) * (max - min) + min;
    win->showTipAt(win->x + x, int(win->y + win->marginTop + 4 * win->dpi + 0.5), std::format(L"{}", std::round(val)));
    win->refresh();
}
void ToolSubSlider::mouseMove(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return;
    float val = ((float)x - start) / (end - start) * (max - min) + min;
    win->showTipAt(win->x + x, int(win->y + win->marginTop + 4 * win->dpi + 0.5), std::format(L"{}", std::round(val)));
}

bool ToolSubSlider::mouseDown(const int& x, const int& y)
{
    if (!visible || x < start || x > end) return false;
    val = ((float)x - start) / (end - start) * (max - min) + min;
    ShapeTextWin::get()->changeState();
    return true;
}

void ToolSubSlider::setVals(const float& max, const float& min, const float& val, const float& start, bool visible)
{
    this->max = max;
    this->min = min;
    this->val = val;
    this->start = start;
    this->visible = visible;
    this->end = start + 80 * win->dpi;
}
