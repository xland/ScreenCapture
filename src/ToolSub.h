#pragma once
#include "include/core/SkPath.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ~ToolSub();
    static void Init();
    static ToolSub *Get();
    bool OnMouseDown(const int& x, const int& y) override;
    void InitBtns(int mainToolSelectedIndex);
    bool OnPaint(SkCanvas* canvas) override;
    bool GetFill();
    int GetStroke();
    SkColor GetColor();

private:
    ToolSub();
    void addStrokeWidthBtns(int index);
    void addColorBtns();
    void setRect();
    SkPath p;
};