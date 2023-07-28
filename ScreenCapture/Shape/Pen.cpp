#include "Pen.h"
namespace Shape {
	void Pen::Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2)
	{
		context->setCompOp(BL_COMP_OP_SRC_OVER);
		context->setStrokeStyle(color);
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
	}
}