#pragma once
#include "include/core/SkPath.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ~ToolSub();
    static void Init();
    static ToolSub *get();
    bool OnMouseDown(const int& x, const int& y) override;
    void InitBtns(int mainToolSelectedIndex);
    bool OnPaint(SkCanvas* canvas) override;
    bool getFill();
    int getStroke();
    SkColor getColor();

private:
    ToolSub();
    void addStrokeWidthBtns(int index);
    void addColorBtns();
    void setRect();
    SkPath p;
};