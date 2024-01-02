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
    static void init();
    static PixelInfo *get();
    bool OnMouseMove(const int &x, const int &y);

private:
    PixelInfo();
};