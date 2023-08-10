#include "Ellipse.h"
#include "../Util.h"
#include "../MainWin.h"
namespace Shape {

    Ellipse::Ellipse()
    {
        state = State::ellipse;
    }
    Ellipse::~Ellipse()
    {

    }
    void Ellipse::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
    {
        isTemp = false;
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        auto win = MainWin::Get();
        if (win->IsShiftDown) {
            SetBoxByPosSquare(box, x1, y1, x2, y2,draggerIndex);
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

    void Ellipse::MouseInDragger(const double& x, const double& y)
    {
        for (int i = 0; i < 4; i++)
        {
            if (draggers[i].contains(x, y)) {
                LPCTSTR cursor = IDC_SIZENESW;
                switch (i)
                {
                case 0: {
                    tempDraggerX = box.x1;
                    tempDraggerY = box.y1;
                    draggerIndex = i;
                    cursor = IDC_SIZENWSE;
                    break;
                }
                case 1:
                {
                    tempDraggerX = box.x0;
                    tempDraggerY = box.y1;
                    draggerIndex = i;
                    cursor = IDC_SIZENESW;
                    break;
                }
                case 2:
                {
                    tempDraggerX = box.x0;
                    tempDraggerY = box.y0;
                    draggerIndex = i;
                    cursor = IDC_SIZENWSE;
                    break;
                }
                case 3: {
                    tempDraggerX = box.x1;
                    tempDraggerY = box.y0;
                    draggerIndex = i;
                    cursor = IDC_SIZENESW;
                    break;
                }
                }
                ChangeCursor(cursor);
                return;
            }
        }
        if (box.contains(x, y)) {
            draggerIndex = 4;
            ChangeCursor(IDC_SIZEALL);
            return;
        }
        draggerIndex = -1;
        ChangeCursor(IDC_CROSS);
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
        painter->isDrawing = false;
        auto win = MainWin::Get();
        win->state = win->preState;
        InvalidateRect(win->hwnd, nullptr, false);
        return true;
    }


    void Ellipse::DragDragger(const double& x, const double& y)
    {
        switch (draggerIndex)
        {
        case 0: {
            Draw(x, y, tempDraggerX, tempDraggerY);
            break;
        }
        case 1: {
            Draw(tempDraggerX, y, x, tempDraggerY);
            break;
        }
        case 2: {
            Draw(tempDraggerX, tempDraggerY, x, y);
            break;
        }
        case 3: {
            Draw(x, tempDraggerY, tempDraggerX, y);
            break;
        }
        case 4: {
            auto win = MainWin::Get();
            auto xSpan = x - win->MouseDownPos.x;
            auto ySpan = y - win->MouseDownPos.y;
            box.x0 += xSpan;
            box.y0 += ySpan;
            box.x1 += xSpan;
            box.y1 += ySpan;
            Draw(box.x0, box.y0, box.x1, box.y1);
            win->MouseDownPos.x = x;
            win->MouseDownPos.y = y;
            break;
        }
        }
    }
}
