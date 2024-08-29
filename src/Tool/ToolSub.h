#pragma once
#include "include/core/SkPath.h"
#include "ToolBase.h"

class ToolSub : public ToolBase
{
public:
    ToolSub();
    ~ToolSub();
    void Init() override;
    void OnLeftBtnDown(const int& x, const int& y) override;
    void OnCustomMsg(const EventType& type, const uint32_t& msg);
    void OnPaint(SkCanvas* canvas) override;
    void InitBtns(const int& mainBtnId);
    bool GetFill();
    int GetStroke(int index = 1);
    SkColor GetColor();

private:
    void addStrokeWidthBtns(int index);
    void addColorBtns();
    void setRect();
    SkPath p;
};