#include "Line.h"
#include "../MainWin.h"
namespace Shape {
	void Line::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
		auto context = Painter::Get()->paintCtx;
		context->begin(*Painter::Get()->prepareImage);
		context->clearAll();
		if (isFill) {
			context->setStrokeStyle(color);
		}
		else
		{			
			context->setStrokeStyle(BLRgba32(color.r(), color.g(), color.b(), 110));
		}		
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
		context->end();
		InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
	}
    bool Line::EndDraw()
    {
        auto painter = Painter::Get();
        if (!painter->isDrawing) {
            return true;
        }
        if (draggerIndex != -1) {
            return false;
        }
        auto context = painter->paintCtx;
        context->begin(*painter->canvasImage);
        if (isFill) {
            context->setStrokeStyle(color);
        }
        else
        {
            context->setStrokeStyle(BLRgba32(color.r(), color.g(), color.b(), 110));
        }
        context->setStrokeWidth(strokeWidth);
        context->setStrokeCaps(BL_STROKE_CAP_ROUND);
        context->strokeLine(x2, y2, x1, y1);
        context->end();
        context->begin(*painter->prepareImage);
        context->clearAll();
        context->end();
        isTemp = false;
        painter->isDrawing = false;
        auto win = MainWin::Get();
        win->state = win->preState;
        InvalidateRect(win->hwnd, nullptr, false);
        return true;
    }
    void Line::ShowDragger()
    {
        draggers[0].x0 = x1 - draggerSize;
        draggers[0].y0 = y1 - draggerSize;
        draggers[0].x1 = x1 + draggerSize;
        draggers[0].y1 = y1 + draggerSize;

        draggers[1].x0 = x2 - draggerSize;
        draggers[1].y0 = y2 - draggerSize;
        draggers[1].x1 = x2 + draggerSize;
        draggers[1].y1 = y2 + draggerSize;

        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 2);
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
    }
    void Line::MouseInDragger(const double& x, const double& y)
    {
        if (draggers[0].contains(x, y)) {
            tempDraggerX = x2;
            tempDraggerY = y2;
            draggerIndex = 0;
            ChangeCursor(IDC_SIZEALL);
        }
        else if (draggers[1].contains(x, y)) {
            tempDraggerX = x1;
            tempDraggerY = y1;
            draggerIndex = 1;
            ChangeCursor(IDC_SIZEALL);
        }
        else
        {
            draggerIndex = -1;
            ChangeCursor(IDC_CROSS);
        }
    }
    void Line::DragDragger(const double& x, const double& y)
    {
        if (draggerIndex == 0) {
            Draw(tempDraggerX, tempDraggerY, x, y);
        }
        else if (draggerIndex == 1) {
            Draw(x, y, tempDraggerX, tempDraggerY);
        }
    }
}