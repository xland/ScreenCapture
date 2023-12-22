#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"
#include <vector>

class ShapeEraser : public ShapeBase
{
public:
    ShapeEraser(const int &x, const int &y);
    ~ShapeEraser();
    bool OnPaint(SkCanvas *canvas) override;

protected:
    bool OnMouseDown(const int& x, const int& y) override;
    bool OnMouseUp(const int& x, const int& y) override;
    bool OnMouseMove(const int& x, const int& y) override;
    bool OnMoseDrag(const int& x, const int& y) override;

private:
    SkPath path;
    bool stroke{ true };
    int strokeWidth{ 4 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void initParams();
};