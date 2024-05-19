#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "ToolBtn.h"
#include <vector>

class ToolBase
{
public:
    ToolBase();
    ~ToolBase();
    bool OnMouseMove(const int &x, const int &y);
    bool OnMouseUp(const int& x, const int& y);
    bool OnMouseDrag(const int& x, const int& y);
    virtual bool OnMouseDown(const int& x, const int& y)=0;
    virtual bool OnPaint(SkCanvas* canvas)=0;
    SkRect ToolRect;
    const static int MarginTop{8};
    int IndexHovered;
    int IndexSelected;
    std::vector<std::shared_ptr<ToolBtn>> Btns;

protected:
    bool isMouseDown{ false };
private:
};