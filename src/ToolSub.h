#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ~ToolSub();
    static void init();
    static ToolSub *get();
    bool OnMouseDown(int x, int y) override;
    void InitBtns(int mainToolSelectedIndex);
    bool OnPaint(SkCanvas* canvas) override;

private:
    ToolSub();
    void addStrokeWidthBtns(int index);
    void addColorBtns();
};