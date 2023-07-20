#pragma once
#include "Shape.h"
namespace Shape {
    class Number : public Shape
    {
    public:
        void Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2) override {};
    };
}


