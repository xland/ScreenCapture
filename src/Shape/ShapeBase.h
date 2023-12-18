#pragma once

#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class ShapeBase
{
public:
    ShapeBase();
    ~ShapeBase();
    virtual bool OnMouseDown(const int& x, const int& y) = 0;
    virtual bool OnMouseMove(const int& x, const int& y) = 0;
    virtual bool OnMouseUp(const int& x, const int& y) = 0;
    virtual bool OnPaint(SkCanvas* canvas) = 0;
    virtual bool OnCheckHover(const int& x, const int& y) { return false; };
    bool IsWIP;
    int startX, startY;
private:
};