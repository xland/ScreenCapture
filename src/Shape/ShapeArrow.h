#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"
#include <vector>

class ShapeArrow : public ShapeBase
{
public:
    ShapeArrow(const int &x, const int &y);
    ~ShapeArrow();
    bool OnMouseDown(const int &x, const int &y) override;
    bool OnMouseUp(const int &x, const int &y) override;
    bool OnMouseMove(const int &x, const int &y) override;
    virtual bool OnPaint(SkCanvas *canvas) override;
    bool OnMoseDrag(const int &x, const int &y) override;

protected:
    virtual bool isMouseOnBorder(const int &x, const int &y);
    void paintDragger(SkCanvas *canvas);
    void initParams();
    SkPath path;
    bool stroke{true};
    int strokeWidth{4};
    SkColor color{SkColorSetARGB(255, 207, 19, 34)};
    std::vector<SkRect> draggers;

private:
    void setCursor();
};