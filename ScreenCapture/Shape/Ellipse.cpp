#include "Ellipse.h"
#include "../Util.h"

namespace Shape {
    void Ellipse::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
    {
        //todo °´×¡Shift»­ÕýÔ²
        BLBox box;
        SetBoxByPos(box, x1, y1, x2, y2);
        ellipse.rx = (x2 - x1) / 2;
        ellipse.ry = (y2 - y1) / 2;
        ellipse.cx = x1 + ellipse.rx;
        ellipse.cy = y1 + ellipse.ry;
        if (isFill)
        {
            paintCtx->setFillStyle(color);
            paintCtx->fillEllipse(ellipse);
        }
        else
        {
            paintCtx->setStrokeStyle(color);
            paintCtx->setStrokeWidth(strokeWidth);
            paintCtx->strokeEllipse(ellipse);
        }
    }
}
