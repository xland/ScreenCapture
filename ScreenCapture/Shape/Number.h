#pragma once
#include "Shape.h"
namespace Shape {
    class Number : public Shape
    {
    public:
        Number();
        ~Number();
        void SetNumber();
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        void ShowDragger() override;
        void MouseInDragger(const double& x, const double& y) override;
        void DragDragger(const double& x, const double& y) override;
        BLRgba32 color;
        double strokeWidth;
        unsigned int number;
    private:
        BLBox draggers[2];
        double degree;
        double tempDraggerX, tempDraggerY;
        int draggerIndex = -1;
        BLPath path;
        BLCircle circle;
    };
}


