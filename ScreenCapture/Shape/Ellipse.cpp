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
        BLEllipse ellipse;
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
    void Ellipse::ShowDragger()
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

        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 4);
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
    }

    bool Ellipse::EndDraw()
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
        BLEllipse ellipse;
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
}
