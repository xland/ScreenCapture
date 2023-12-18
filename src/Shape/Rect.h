#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"
#include <vector>

class Rect : public ShapeBase
{
public:
    Rect();
    ~Rect();
    bool OnMouseDown(const int& x, const int& y);
    bool OnMouseUp(const int& x, const int& y);
    bool OnMouseMove(const int& x, const int& y);
    bool OnPaint(SkCanvas *canvas);
    bool OnCheckHover(const int& x, const int& y);

private:
    void initParams();
    std::vector<SkRect> draggers;
    SkRect rect;
    bool stroke{ true };
    int strokeWidth{ 4 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
};