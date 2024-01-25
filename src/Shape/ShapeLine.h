#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"

class ShapeLine : public ShapeBase
{
public:
    ShapeLine(const int &x, const int &y);
    ~ShapeLine();
    bool OnMouseDown(const int &x, const int &y) override;
    bool OnMouseMove(const int &x, const int &y) override;
    bool OnMouseUp(const int &x, const int &y) override;
    bool OnMoseDrag(const int &x, const int &y) override;
    void Paint(SkCanvas *canvas) override;

protected:
private:
    void initParams();
    SkPath path;
    SkPath pathAssist;
    bool fill{ false };
    int strokeWidth{ 4 };
    int hoverX, hoverY;
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
};