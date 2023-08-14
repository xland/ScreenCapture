#include "Eraser.h"
#include "../MainWin.h"
namespace Shape {

	Eraser::Eraser()
	{
		state = State::eraser;
        
	}
	Eraser::~Eraser()
	{
        delete canvasImgCopy;
	}


    void Eraser::CopyCanvasImg()
    {
        if (!isFill) return;
        auto painter = Painter::Get();
        auto paintCtx = painter->paintCtx;
        canvasImgCopy = new BLImage(*painter->canvasImage);
        paintCtx->begin(*painter->prepareImage);
        paintCtx->blitImage(BLRect(0, 0, painter->w, painter->h), *painter->canvasImage);
        paintCtx->end();
        paintCtx->begin(*painter->canvasImage);
        paintCtx->clearAll();
        paintCtx->end();
    }

	void Eraser::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto painter = Painter::Get();
		auto context = painter->paintCtx;
        BLRgba32 black(0, 0, 0);
        auto win = MainWin::Get();
        if (isFill) {
            if (x1 == -1) {
                if (!canvasImgCopy) {
                    CopyCanvasImg();
                }
            }
            else {
                SetBoxByPos(box, x1, y1, x2, y2);
            }
            context->begin(*painter->prepareImage);
            context->clearAll();
            context->blitImage(BLRect(0, 0, painter->w, painter->h), *canvasImgCopy);
            context->setCompOp(BL_COMP_OP_CLEAR);
            context->setFillStyle(black);
            context->fillBox(box);
            context->setCompOp(BL_COMP_OP_SRC_OVER);
            context->setStrokeStyle(black);
            context->setStrokeWidth(1);
            context->strokeBox(box);
            context->end();
        }
        else
        {
            context->begin(*painter->canvasImage);
            context->setCompOp(BL_COMP_OP_CLEAR);
            context->setStrokeStyle(black);
            context->setStrokeWidth(strokeWidth);
            context->setStrokeCaps(BL_STROKE_CAP_ROUND);
            if (x1 == -1) {
                for (size_t i = 0; i < points.size(); i++)
                {
                    if (i + 1 >= points.size())break;
                    context->strokeLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
                }
            }
            else
            {
                context->strokeLine(x2, y2, x1, y1);
                if (points.size() < 1) {
                    points.push_back(BLPoint(x2, y2));
                }
                points.push_back(BLPoint(x1, y1));
            }
            context->end();
            win->MouseDownPos.x = (LONG)x1;
            win->MouseDownPos.y = (LONG)y1;
        }
		InvalidateRect(win->hwnd, nullptr, false);		
	}
	bool Eraser::EndDraw()
	{
        if (draggerIndex != -1) {
            return false;
        }
        auto painter = Painter::Get();
        if (!painter->isDrawing) {
            return true;
        }
        auto context = painter->paintCtx;
        BLRgba32 black(0, 0, 0);
        auto win = MainWin::Get();
        if (isFill) {
            context->begin(*painter->canvasImage);
            if (canvasImgCopy) {
                context->blitImage(BLRect(0, 0, painter->w, painter->h), *canvasImgCopy);
                win->state = win->preState;
                delete canvasImgCopy;
                canvasImgCopy = nullptr;
                static int i = 1;
                Debug(std::to_string(i++));
            }
            context->setCompOp(BL_COMP_OP_CLEAR);
            context->setFillStyle(black);
            context->fillBox(box);
            context->end();
            context->begin(*painter->prepareImage);
            context->clearAll();
            context->end();
        }
        else
        {
            context->begin(*painter->canvasImage);
            context->setCompOp(BL_COMP_OP_CLEAR);
            context->setStrokeStyle(black);
            context->setStrokeWidth(strokeWidth);
            context->setStrokeCaps(BL_STROKE_CAP_ROUND);
            for (size_t i = 0; i < points.size(); i++)
            {
                if (i + 1 >= points.size())break;
                context->strokeLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
            }
            context->end();
        }
		painter->isDrawing = false;
		InvalidateRect(win->hwnd, nullptr, false);
		return true;
	}

    void Eraser::ShowDragger()
    {
        auto painter = Painter::Get();
        auto context = painter->paintCtx;
        auto win = MainWin::Get();
        if (!isFill)
        {
            return;
        }
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

        context->begin(*painter->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 4);
        context->end();
        win->state = State::lastPathDrag;
        InvalidateRect(win->hwnd, nullptr, false);
    }

    void Eraser::MouseInDragger(const double& x, const double& y)
    {
        if (!isFill)
        {
            ChangeCursor(IDC_CROSS);
            return;
        }
        for (size_t i = 0; i < 4; i++)
        {
            if (draggers[i].contains(x, y)) {
                LPCTSTR cursor = IDC_SIZENESW;
                switch (i)
                {
                case 0: {
                    tempDraggerX = box.x1;
                    tempDraggerY = box.y1;
                    draggerIndex = (int)i;
                    cursor = IDC_SIZENWSE;
                    break;
                }
                case 1:
                {
                    tempDraggerX = box.x0;
                    tempDraggerY = box.y1;
                    draggerIndex = (int)i;
                    cursor = IDC_SIZENESW;
                    break;
                }
                case 2:
                {
                    tempDraggerX = box.x0;
                    tempDraggerY = box.y0;
                    draggerIndex = (int)i;
                    cursor = IDC_SIZENWSE;
                    break;
                }
                case 3: {
                    tempDraggerX = box.x1;
                    tempDraggerY = box.y0;
                    draggerIndex = (int)i;
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
    void Eraser::DragDragger(const double& x, const double& y)
    {
        if (!isFill) return;
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
            auto x0 = x - win->MouseDownPos.x + box.x0;
            auto y0 = y - win->MouseDownPos.y + box.y0;
            auto x1 = x - win->MouseDownPos.x + box.x1;
            auto y1 = y - win->MouseDownPos.y + box.y1;
            win->MouseDownPos.x = x;
            win->MouseDownPos.y = y;
            if (x0<0 || y0<0 || x1>Painter::Get()->w || y1>Painter::Get()->h) return;
            box.x0 = x0;
            box.y0 = y0;
            box.x1 = x1;
            box.y1 = y1;
            Draw(box.x0, box.y0, box.x1, box.y1);
            break;
        }
        }
    }
}