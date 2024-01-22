#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
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
    float MarginTop{8.0f};
    int IndexHovered;
    int IndexSelected;
    std::vector<std::shared_ptr<ToolBtn>> Btns;

protected:
    bool isMouseDown{ false };
private:
};