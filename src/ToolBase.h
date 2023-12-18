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
    virtual bool OnMouseDown(const int& x, const int& y)=0;
    virtual bool OnPaint(SkCanvas* canvas)=0;
    SkRect ToolRect;
    int MarginTop{8};
    int IndexHovered;
    int IndexSelected;

protected:
    std::vector<std::shared_ptr<ToolBtn>> btns;

private:
};