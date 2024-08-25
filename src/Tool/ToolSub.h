#pragma once
#include "include/core/SkPath.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ToolSub();
    ~ToolSub();
    static void Init();
    static ToolSub* Get();
    bool OnMouseDown(const int& x, const int& y) override;
    void InitBtns(int mainToolSelectedIndex);
    bool Paint(SkCanvas* canvas) override;
    bool GetFill();
    int GetStroke(int index = 1);
    SkColor GetColor();

private:
    void addStrokeWidthBtns(int index);
    void addColorBtns();
    void setRect();
    SkPath p;
};