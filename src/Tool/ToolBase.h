#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "ToolBtn.h"
#include <vector>
#include "../EventHandler.h"

class ToolBase:public EventHandler
{
public:
    ToolBase();
    ~ToolBase();
    virtual void Init() = 0;
    void OnMouseMove(const int& x, const int& y);
    virtual void OnLeftBtnDown(const int& x, const int& y) = 0;
    virtual void OnPaint(SkCanvas* canvas) = 0;
    SkRect toolRect;
    const static int MarginTop{ 8 };
    int indexHovered;
    int indexSelected;
    std::vector<ToolBtn> Btns;

protected:

private:
};