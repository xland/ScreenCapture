#pragma once
#include "Shape.h"
namespace Shape {
    class Pen : public Shape
    {
    public:
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        std::vector<BLPoint> points;
        double strokeWidth;
        BLRgba32 color;
    };
}


