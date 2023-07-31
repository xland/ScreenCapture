#include "Box.h"
#include "../Util.h"

namespace Shape {
	void Box::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        SetBoxByPos(box,x1, y1,x2, y2);
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
	}

    void Box::ShowDragger(BLContext* context)
    {
        static int draggerSize = 6;
        boxes[0].x0 = box.x0 - draggerSize;
        boxes[0].y0 = box.y0 - draggerSize;
        boxes[0].x1 = box.x0 + draggerSize;
        boxes[0].y1 = box.y0 + draggerSize;

        boxes[1].x0 = box.x1 - draggerSize;
        boxes[1].y0 = box.y0 - draggerSize;
        boxes[1].x1 = box.x1 + draggerSize;
        boxes[1].y1 = box.y0 + draggerSize;

        boxes[2].x0 = box.x1 - draggerSize;
        boxes[2].y0 = box.y1 - draggerSize;
        boxes[2].x1 = box.x1 + draggerSize;
        boxes[2].y1 = box.y1 + draggerSize;

        boxes[3].x0 = box.x0 - draggerSize;
        boxes[3].y0 = box.y1 - draggerSize;
        boxes[3].x1 = box.x0 + draggerSize;
        boxes[3].y1 = box.y1 + draggerSize;
    }
}
