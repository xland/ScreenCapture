#pragma once
#include "Shape.h"
namespace Shape {
    class Number : public Shape
    {
    public:
        Number();
        void Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2) override;
        double degree;
        BLRgba32 color;
        BLPoint centerPoint;
        double r;
        unsigned int number;
        double strokeWidth;
        bool isFill = false;
    };
}


