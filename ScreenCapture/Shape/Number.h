#pragma once
#include "Shape.h"
namespace Shape {
    class Number : public Shape
    {
    public:
        Number();
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        void ShowDragger() override;
        void MouseInDragger(const double& x, const double& y) override;
        void DragDragger(const double& x, const double& y) override;
        BLPath path;
        BLCircle circle;
        double degree;
        BLRgba32 color;
        unsigned int number;
        double strokeWidth;
        bool isFill = false;
        BLBox draggers[2];
    private:
        double tempDraggerX, tempDraggerY;
        int draggerIndex = -1;
    };
}


