#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"

class ShapeEraser : public ShapeBase
{
public:
    ShapeEraser(const int &x, const int &y);
    ~ShapeEraser();
    void Paint(SkCanvas *canvas) override;

protected:
    bool OnMouseUp(const int& x, const int& y) override;
    bool OnMoseDrag(const int& x, const int& y) override;

private:
    SkPath path;
    bool stroke{ true };
    int strokeWidth{ 26 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void initParams();
};