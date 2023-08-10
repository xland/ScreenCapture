#pragma once
#include "Shape.h"
namespace Shape {
    class Pen : public Shape
    {
    public:
        Pen();
        ~Pen();
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        double strokeWidth;
        BLRgba32 color;
    private:
        std::vector<BLPoint> points;
    };
}


