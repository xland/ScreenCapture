#pragma once
#include "Shape.h"
namespace Shape {
    class Line : public Shape
    {
    public:
        Line();
        ~Line();
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        void ShowDragger() override;
        void MouseInDragger(const double& x, const double& y) override;
        void DragDragger(const double& x, const double& y) override;
        double strokeWidth;
        BLRgba32 color;
        double x1, y1, x2, y2;
    private:
        double tempDraggerX, tempDraggerY;
        int draggerIndex = -1;
        BLBox draggers[2];
    };
}


