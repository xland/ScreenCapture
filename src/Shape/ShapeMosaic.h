#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"

class ShapeMosaic : public ShapeBase
{
public:
    ShapeMosaic(const int &x, const int &y);
    ~ShapeMosaic();
    void Paint(SkCanvas *canvas) override;

protected:
    bool OnMouseDown(const int &x, const int &y) override;
    bool OnMouseUp(const int &x, const int &y) override;
    bool OnMoseDrag(const int &x, const int &y) override;

private:
    void initParams();
    void drawRectsByPoints(const std::vector<SkPoint>& points, SkCanvas *canvas);
    SkPath path;
    bool stroke{true};
    int strokeWidth{26};
    float size{20};
};