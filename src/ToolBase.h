#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include "ToolBtn.h"
#include <vector>

class ToolBase : public EventHandler
{
public:
    ToolBase();
    ~ToolBase();
    bool OnMouseMove(int x, int y) override;
    SkRect ToolRect;
    int MarginTop{8};
    int IndexHovered;
    int IndexSelected;
protected:
    std::vector<std::shared_ptr<ToolBtn>> btns;
private:
};