#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"
#include <map>

class ShapeMosaic : public ShapeBase
{
public:
    ShapeMosaic(const int &x, const int &y);
    ~ShapeMosaic();
    bool OnPaintFinish(SkCanvas* canvas) override { return false; };
    void Paint(SkCanvas *canvas) override;

protected:
    bool OnMouseDown(const int &x, const int &y) override;
    bool OnMouseUp(const int &x, const int &y) override;
    bool OnMouseMove(const int &x, const int &y) override;
    bool OnMoseDrag(const int &x, const int &y) override;

private:
    void initParams();
    void drawRectsByPoints(const SkPoint& point, SkCanvas* canvas);
    SkPath path;
    bool stroke{true};
    int strokeWidth{26};
    float size{ 20 };
    SkColor color{SkColorSetARGB(255, 207, 19, 34)};
    std::map<int, SkColor> colorCache;
    SkPixmap* pixmap;
};