#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class PixelInfo
{
public:
    ~PixelInfo();
    static void Init();
    static PixelInfo *Get();
    bool OnPaint(SkCanvas* canvas);
    bool OnMouseMove(const int& x, const int& y);

private:
    PixelInfo();
    SkRect rect;
};