#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include <vector>

class ToolBase : public EventHandler
{
public:
    ToolBase();
    ~ToolBase();
    SkRect toolRect;
    int indexHover{-1};
    int indexSelected{-1};
    int marginTop{12};
    int btnWidth{50};
    int height{46};
    int iconLeftMargin{14};
protected:
    void drawBgRect(SkCanvas* canvas,SkPaint& paint);
    void drawBtnCheckable(SkCanvas* canvas, SkPaint& paint,std::initializer_list<const char*>& btns);
private:
};