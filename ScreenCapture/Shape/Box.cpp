#include "Box.h"

namespace Shape {
	void Box::Draw(BLContext* paintCtx)
	{
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
