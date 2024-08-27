#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "ToolBtn.h"
#include <vector>
#include "../EventType.h"
#include <vector>

class ToolBase
{
public:
    ToolBase();
    ~ToolBase();
    void onMouseMove(const int& x, const int& y);
    void onLeftBtnUp(const int& x, const int& y);
    void onMouseDrag(const int& x, const int& y);
    virtual void onCustomMsg(const EventType& type, const uint32_t& msg) = 0;
    virtual void onLeftBtnDown(const int& x, const int& y) = 0;
    virtual void Paint(SkCanvas* canvas) = 0;
    SkRect ToolRect;
    const static int MarginTop{ 8 };
    int IndexHovered;
    int IndexSelected;
    std::vector<ToolBtn> Btns;

protected:
    bool isMouseDown{ false };
private:
};