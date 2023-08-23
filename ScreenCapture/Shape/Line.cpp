#include "Line.h"
#include "../WindowBase.h"
namespace Shape {

    Line::Line()
    {
        state = State::line;
    }
    Line::~Line()
    {

    }
	void Line::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto win = WindowBase::Get();
		auto context = win->PaintCtx;
		context->begin(*win->PrepareImage);
		context->clearAll();
		if (IsFill) {
			context->setStrokeStyle(color);
		}
		else
		{			
			context->setStrokeStyle(BLRgba32(color.r(), color.g(), color.b(), 110));
		}		
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
        if (x1 == -1) {
            context->strokeLine(this->x2, this->y2, this->x1, this->y1);
        }
        else
        {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
            context->strokeLine(x2, y2, x1, y1);
        }		
		context->end();
        win->Refresh();
	}
    bool Line::EndDraw()
    {
        auto win = WindowBase::Get();
        if (!win->IsDrawing) {
            return true;
        }
        if (draggerIndex != -1) {
            return false;
        }
        auto context = win->PaintCtx;
        context->begin(*win->CanvasImage);
        if (IsFill) {
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
        context->begin(*win->PrepareImage);
        context->clearAll();
        context->end();
        win->IsDrawing = false;
        win->state = win->PreState;
        win->Refresh();
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
        auto win = WindowBase::Get();
        auto context = win->PaintCtx;
        context->begin(*win->PrepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 2);
        context->end();
        win->Refresh();
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