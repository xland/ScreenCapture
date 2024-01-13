#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeRect.h"
#include <vector>

class ShapePen : public ShapeBase
{
public:
    ShapePen(const int &x, const int &y);
    ~ShapePen();
    bool OnMoseDrag(const int& x, const int& y) override;
    void Paint(SkCanvas *canvas) override;

protected:

private:
    SkPath path;
    bool stroke{ true };
    int strokeWidth{ 4 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void initParams();
};