#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "ToolBase.h"
#include "ToolBtn.h"
#include <vector>

class ToolMain : public ToolBase
{
public:
    ~ToolMain();
    static void init();
    static ToolMain *get();
    bool OnMouseDown(int x, int y) override;
    bool OnPaint(SkCanvas *canvas) override;
private:
    ToolMain();
};