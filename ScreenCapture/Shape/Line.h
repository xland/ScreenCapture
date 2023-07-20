#pragma once
#include "Shape.h"
namespace Shape {
    class Line : public Shape
    {
    public:
        void Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2) override;
        double strokeWidth;
        BLRgba32 color;
        BLPoint point1;
        BLPoint point2;
    };
}


