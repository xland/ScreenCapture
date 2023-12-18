#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class CutMask
{
public:
    ~CutMask();
    static void init();
    static CutMask* get();
    bool OnMouseDown(const int& x, const int& y);
    bool OnMouseUp(const int& x, const int& y);
    bool OnMouseMove(const int& x, const int& y);
    bool OnMouseDrag(const int& x, const int& y);
    bool OnPaint(SkCanvas* canvas);
    SkRect CutRect;

private:
    CutMask();
    SkPoint start;
    SkPath path;
};