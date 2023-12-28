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
    bool OnMouseDown(const int& x, const int& y) override;
    bool OnMouseMove(const int& x, const int& y) override;
    bool OnMouseUp(const int& x, const int& y) override;
    bool OnMoseDrag(const int& x, const int& y) override;
    void Paint(SkCanvas *canvas) override;

protected:

private:
    SkPath path;
    bool stroke{ true };
    int strokeWidth{ 4 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void initParams();
    void setDragger();
};