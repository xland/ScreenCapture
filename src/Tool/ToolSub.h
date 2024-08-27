#pragma once
#include "include/core/SkPath.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ToolSub();
    ~ToolSub();
    void Init() override;
    static ToolSub* Get();
    void OnLeftBtnDown(const int& x, const int& y) override;
    void OnPaint(SkCanvas* canvas) override;
    void InitBtns(int mainToolSelectedIndex);
    bool GetFill();
    int GetStroke(int index = 1);
    SkColor GetColor();

private:
    void addStrokeWidthBtns(int index);
    void addColorBtns();
    void setRect();
    SkPath p;
};