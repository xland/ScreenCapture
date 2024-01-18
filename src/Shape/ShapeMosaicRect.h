#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeRect.h"
#include <vector>
#include <map>

class ShapeMosaicRect : public ShapeRect
{
public:
    ShapeMosaicRect(const int &x, const int &y);
    ~ShapeMosaicRect();
    void Paint(SkCanvas *canvas) override;
protected:
    bool OnMouseMove(const int& x, const int& y) override;
    bool OnMouseDown(const int& x, const int& y) override;
    bool OnMouseUp(const int& x, const int& y) override;
    void OnShowDragger(SkCanvas* canvas) override;
private:
    void drawRectsByPoints(SkCanvas* canvas);
    std::map<int, SkColor> colorCache;
    float size{ 20 };
};