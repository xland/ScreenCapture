#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "ToolBtn.h"
#include <vector>
#include "../EventHandler.h"

class ToolBase:public EventHandler
{
public:
    ToolBase();
    ~ToolBase();
    virtual void Init() = 0;
    void OnMouseMove(const int& x, const int& y);
    void OnLeftBtnUp(const int& x, const int& y);
    void OnMouseDrag(const int& x, const int& y);
    virtual void OnCustomMsg(const EventType& type, const uint32_t& msg) = 0;
    virtual void OnLeftBtnDown(const int& x, const int& y) = 0;
    virtual void OnPaint(SkCanvas* canvas) = 0;
    SkRect ToolRect;
    const static int MarginTop{ 8 };
    int IndexHovered;
    int IndexSelected;
    std::vector<ToolBtn> Btns;

protected:
    bool isMouseDown{ false };
private:
};