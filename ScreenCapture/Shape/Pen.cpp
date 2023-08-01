#include "Pen.h"
#include "../MainWin.h"
namespace Shape {
	void Pen::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
		auto context = Painter::Get()->paintCtx;
		context->begin(*Painter::Get()->prepareImage);
		context->setCompOp(BL_COMP_OP_SRC_OVER);
		context->setStrokeStyle(color);
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
		context->end();
		auto win = MainWin::Get();
		win->MouseDownPos.x = (LONG)x1;
		win->MouseDownPos.y = (LONG)y1;
		InvalidateRect(win->hwnd, nullptr, false);
	}
}