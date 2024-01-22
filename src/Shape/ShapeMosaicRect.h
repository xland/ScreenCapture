#pragma once
#include "include/core/SkCanvas.h"
#include "ShapeRect.h"

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
    float size{ 20 };
};