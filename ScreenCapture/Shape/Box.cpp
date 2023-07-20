#include "Box.h"
#include "../Util.h"

namespace Shape {
	void Box::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
	{
        SetBoxByPos(box,x1, y1,x2, y2);

        //todo 按住Shift画正方
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
