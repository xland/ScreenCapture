#pragma once
#include "include/core/SkCanvas.h"
#include "ShapeRect.h"
#include <vector>

class ShapeEraserRect : public ShapeRect
{
public:
    ShapeEraserRect(const int &x, const int &y);
    ~ShapeEraserRect();
    void Paint(SkCanvas *canvas) override;
    void OnShowDragger(SkCanvas* canvas) override;
protected:
    bool OnMouseMove(const int& x, const int& y) override;
private:
};