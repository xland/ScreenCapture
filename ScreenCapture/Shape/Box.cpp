#include "Box.h"
#include "../Util.h"
#include "../MainWin.h"

namespace Shape {
	void Box::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        auto win = MainWin::Get();
        if (win->IsShiftDown) {
            SetBoxByPosSquare(box, x1, y1, x2, y2);
        }
        else
        {
            SetBoxByPos(box, x1, y1, x2, y2);
        }        
        //todo 按住Shift画正方
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillBox(box);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokeBox(box);
        }
        context->end();
        InvalidateRect(win->hwnd, nullptr, false);        
	}

    void Box::ShowDragger()
    {
        static int draggerSize = 6;
        draggers[0].x0 = box.x0 - draggerSize;
        draggers[0].y0 = box.y0 - draggerSize;
        draggers[0].x1 = box.x0 + draggerSize;
        draggers[0].y1 = box.y0 + draggerSize;

        draggers[1].x0 = box.x1 - draggerSize;
        draggers[1].y0 = box.y0 - draggerSize;
        draggers[1].x1 = box.x1 + draggerSize;
        draggers[1].y1 = box.y0 + draggerSize;

        draggers[2].x0 = box.x1 - draggerSize;
        draggers[2].y0 = box.y1 - draggerSize;
        draggers[2].x1 = box.x1 + draggerSize;
        draggers[2].y1 = box.y1 + draggerSize;

        draggers[3].x0 = box.x0 - draggerSize;
        draggers[3].y0 = box.y1 - draggerSize;
        draggers[3].x1 = box.x0 + draggerSize;
        draggers[3].y1 = box.y1 + draggerSize;

        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
    }
}
