#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include <vector>

class ToolMain : public EventHandler
{
public:
    ~ToolMain();
    static void init();
    static ToolMain *get();
    bool OnMouseDown(int x, int y) override;
    bool OnMouseUp(int x, int y) override;
    bool OnMouseMove(int x, int y) override;
    bool OnPaint(SkCanvas* canvas) override;
private:
    ToolMain();
    int btnCount = 15;
    int toolBoxSpan = 12;//工具条距离截图区域的高度
    int toolBtnWidth = 50;
    int toolBoxWidth = btnCount * toolBtnWidth;
    int toolBoxHeight = 46;
    int iconLeftMargin = 14;
    SkRect toolMainRect;
    int indexHover {-1};
    int indexSelected{ -1 };
};