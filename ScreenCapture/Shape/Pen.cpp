#include "Pen.h"
#include "../MainWin.h"
namespace Shape {
	void Pen::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
		isTemp = false;
		auto context = Painter::Get()->paintCtx;
		context->begin(*Painter::Get()->canvasImage); //直接画，不然上一步干不掉它
		context->setStrokeStyle(color);
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
		context->end();
		auto win = MainWin::Get();
		win->MouseDownPos.x = (LONG)x1;
		win->MouseDownPos.y = (LONG)y1;
		InvalidateRect(win->hwnd, nullptr, false);
		if (points.size() < 1) {
			points.push_back(BLPoint(x2, y2));
		}
		points.push_back(BLPoint(x1, y1));
	}
    bool Pen::EndDraw()
    {
        auto painter = Painter::Get();
        auto context = painter->paintCtx;
        context->begin(*painter->canvasImage);
		context->setStrokeStyle(color);
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		for (size_t i = 0; i < points.size(); i++)
		{
			if (i + 1 >= points.size())break;
			context->strokeLine(points[i].x, points[i].y, points[i+1].x, points[i + 1].y);
		}		
		context->end();
        auto win = MainWin::Get();
		painter->isDrawing = false;
		//Debug("aaa state:" + std::to_string((int)win->state));
        InvalidateRect(win->hwnd, nullptr, false);
        return true;
    }
}