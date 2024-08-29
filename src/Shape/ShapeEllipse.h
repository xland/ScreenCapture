#pragma once
#include "include/core/SkCanvas.h"
#include "ShapeRect.h"

class ShapeEllipse : public ShapeRect
{
public:
    ShapeEllipse(const int &x, const int &y);
    ~ShapeEllipse();
    void Paint(SkCanvas *canvas) override;
protected:
    bool OnMouseMove(const int& x, const int& y) override;
private:
};