#include "Ellipse.h"
#include "../Util.h"

namespace Shape {
    void Ellipse::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
    {
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        //todo °´×¡Shift»­ÕıÔ²
        BLBox box;
        SetBoxByPos(box, x1, y1, x2, y2);
        ellipse.rx = (x2 - x1) / 2;
        ellipse.ry = (y2 - y1) / 2;
        ellipse.cx = x1 + ellipse.rx;
        ellipse.cy = y1 + ellipse.ry;
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillEllipse(ellipse);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokeEllipse(ellipse);
        }
        context->end();
    }
}
