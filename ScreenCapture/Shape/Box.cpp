#include "Box.h"
#include "../Util.h"

namespace Shape {
	void Box::Draw(BLContext* paintCtx, double x1, double y1, double x2, double y2)
	{
        SetBoxByPos(box,x1, y1,x2, y2);
        if (isFill)
        {
            paintCtx->setFillStyle(color);
            paintCtx->fillBox(box);
        }
        else
        {
            paintCtx->setStrokeStyle(color);
            paintCtx->setStrokeWidth(strokeWidth);
            paintCtx->strokeBox(box);
        }
	}
}
