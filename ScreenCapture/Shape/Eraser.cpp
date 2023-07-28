#include "Eraser.h"
namespace Shape {
	void Eraser::Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2)
	{
		context->setCompOp(BL_COMP_OP_CLEAR);
		context->setStrokeStyle(BLRgba32(0, 0, 0));
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
	}
}