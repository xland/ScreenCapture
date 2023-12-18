#pragma once

#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class ShapeDragger
{
public:
    ShapeDragger();
    ~ShapeDragger();
    virtual bool OnCheckHover(const int &x, const int &y) { return false; };

private:
};