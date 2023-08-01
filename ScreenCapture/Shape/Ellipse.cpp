#include "Ellipse.h"
#include "../Util.h"
#include "../MainWin.h"
namespace Shape {
    void Ellipse::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
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
        ellipse.rx = (box.x1 - box.x0) / 2;
        ellipse.ry = (box.y1 - box.y0) / 2;
        ellipse.cx = box.x0 + ellipse.rx;
        ellipse.cy = box.y0 + ellipse.ry;
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillEllipse(ellipse);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokeEllipse(ellipse);
        }
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
    }
}
