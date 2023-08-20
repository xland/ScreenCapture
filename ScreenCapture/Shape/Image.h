#pragma once
#include "Shape.h"
namespace Shape {
    class Image : public Shape
    {
    public:
        void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
        bool EndDraw() override;
        void ShowDragger() override;
        void MouseInDragger(const double& x, const double& y) override;
        void DragDragger(const double& x, const double& y) override;
    private:
        BLBox box;
        BLBox draggers[2];
        double tempDraggerX, tempDraggerY;
        int draggerIndex = -1;
    };
}

