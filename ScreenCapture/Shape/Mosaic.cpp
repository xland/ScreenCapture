#include "Mosaic.h"
#include "../Util.h"
#include "../WindowBase.h"

namespace Shape {

    Mosaic::Mosaic()
    {
        state = State::mosaic;
    }
    Mosaic::~Mosaic()
    {
    }
    void Mosaic::setSamplingPoints(BLPointI* points,const int& x,const int& y)
    {
        //left top point
        points[0].x = x;
        points[0].y = y;
        //right top point
        points[1].y = y;
        points[1].x = x + strokeWidth;
        //center point
        points[2].x = x + strokeWidth / 2;
        points[2].y = y + strokeWidth / 2;
        //left down point
        points[3].x = x;
        points[3].y = y + strokeWidth;
        //right down point
        points[4].x = x + strokeWidth;
        points[4].y = y + strokeWidth;
    }
    void Mosaic::InitMosaicImg()
    {
        auto win = WindowBase::Get();
        auto PaintCtx = win->PaintCtx;
        //先在PrepareImage上贴OriginalImage，再贴CanvasImage
        PaintCtx->begin(*win->PrepareImage);
        if (win->IsMainWin) {
            
            BLPoint startPos(0, 0);
            PaintCtx->blitImage(startPos, *win->OriginalImage);
            PaintCtx->blitImage(startPos, *win->CanvasImage);
        }
        else
        {
            BLPoint startPos(16, 16);
            PaintCtx->blitImage(startPos, *win->OriginalImage);
            BLRectI srcRect(16, 16, win->OriginalImage->width(), win->OriginalImage->height());
            PaintCtx->blitImage(startPos, *win->CanvasImage,srcRect);
        }
        PaintCtx->end();
        //得到PrepareImage上的像素数据
        BLImageData imgData;
        win->PrepareImage->getData(&imgData);
        unsigned char* pixelData = (unsigned char*)(imgData.pixelData);
        //创建MosaicImage，并在MosaicImage上生成马赛克图像（使用的是PrepareImage上的像素数据）
        //以后的repaint，会使用MosaicImage代替CanvaseImage
        win->MosaicImage = new BLImage(imgData.size.w, imgData.size.h, BL_FORMAT_PRGB32);
        PaintCtx->begin(*win->MosaicImage);
        BLPointI points[5];
        for (size_t y = 0; y <= win->MosaicImage->height(); y+=strokeWidth)
        {
            for (size_t x = 0; x <= win->MosaicImage->width(); x+=strokeWidth)
            {
                setSamplingPoints(points, x, y);
                unsigned int b{ 0 }, g{ 0 }, r{ 0 };
                for (size_t i = 0; i < 5; i++)
                {
                    auto index = points[i].y * imgData.stride + points[i].x * 4;
                    if (index > win->dataSize) {
                        break;
                    }
                    b += pixelData[index];
                    g += pixelData[index + 1];
                    r += pixelData[index + 2];
                }
                PaintCtx->setFillStyle(BLRgba32(r / 5, g / 5, b / 5));
                PaintCtx->fillBox(points[0].x, points[0].y, points[4].x, points[4].y);
            }
        }
        PaintCtx->end();
        win->IsMosaicUsePen = !IsFill;
    }
	void Mosaic::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto win = WindowBase::Get();
        if (win->MosaicImage == nullptr) {
            InitMosaicImg();
        }
        auto context = win->PaintCtx;
        context->begin(*win->PrepareImage);    
        win->IsMosaicUsePen = !IsFill;
        if (IsFill) {
            if (x1 != -1) {
                SetBoxByPos(box, x1, y1, x2, y2);
            }
            context->clearAll();
            int w = box.x1 - box.x0;
            int h = box.y1 - box.y0;
            context->blitImage(BLPoint(box.x0, box.y0), *win->MosaicImage, BLRectI((int)box.x0, (int)box.y0, (int)w, (int)h));
            context->setStrokeStyle(BLRgba32(0, 0, 0));
            context->setStrokeWidth(1);
            context->strokeBox(box);
            context->end();
        }
        else
        {
            context->setCompOp(BL_COMP_OP_CLEAR);
            context->setStrokeStyle(BLRgba32(0, 0, 0));
            context->setStrokeWidth(strokeWidth+26);
            context->setStrokeCaps(BL_STROKE_CAP_ROUND);
            if (x1 == -1) {
                for (size_t i = 0; i < points.size(); i++)
                {
                    if (i + 1 >= points.size())break;
                    context->strokeLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
                }
                context->end();
                EndDraw();
            }
            else
            {
                context->strokeLine(x2, y2, x1, y1);
                if (points.size() < 1) {
                    points.push_back(BLPoint(x2, y2));
                }
                points.push_back(BLPoint(x1, y1));                
                win->MouseDownPos.x = (LONG)x1;
                win->MouseDownPos.y = (LONG)y1;
                context->end();
            }
        }
        win->Refresh();
	}

    bool Mosaic::EndDraw()
    {
        auto win = WindowBase::Get();
        auto context = win->PaintCtx;
        if (!win->IsDrawing) {
            return true;
        }
        if (draggerIndex != -1) {
            return false;
        }
        if (IsFill) {
            if (win->MosaicImage == nullptr) {
                InitMosaicImg();
            }
            int w = box.x1 - box.x0;
            int h = box.y1 - box.y0;
            context->begin(*win->CanvasImage);
            context->blitImage(BLPoint(box.x0, box.y0), *win->MosaicImage, BLRectI((int)box.x0, (int)box.y0, (int)w, (int)h));
            context->end();
        }
        else
        {
            if (win->MosaicImage == nullptr) {
                InitMosaicImg();
                Draw(-1, -1, -1, -1);//这里有一个小小的循环调用
                return true; 
            }
            win->IsMosaicUsePen = false;
            box.x0 = 999999999;
            box.y0 = 999999999;
            box.x1 = -1;
            box.y1 = -1;
            for (size_t i = 0; i < points.size(); i++)
            {
                if (points[i].x < box.x0) {
                    box.x0 = points[i].x;
                }
                if (points[i].x > box.x1) {
                    box.x1 = points[i].x;
                }
                if (points[i].y < box.y0) {
                    box.y0 = points[i].y;
                }
                if (points[i].y > box.y1) {
                    box.y1 = points[i].y;
                }
            }
            box.x0 -= strokeWidth*2;
            box.y0 -= strokeWidth*2;
            box.x1 += strokeWidth*2;
            box.y1 += strokeWidth*2;
            if (box.x0 < 0) box.x0 = 0;
            if (box.y0 < 0) box.y0 = 0;
            if (box.x1 > win->w) box.x1 = win->w;
            if (box.y1 > win->h) box.y1 = win->h;
            int w = box.x1 - box.x0;
            int h = box.y1 - box.y0;
            context->begin(*win->CanvasImage);
            context->blitImage(BLPoint(box.x0, box.y0), *win->MosaicImage, BLRectI((int)box.x0, (int)box.y0, (int)w, (int)h));
            context->blitImage(BLPoint(box.x0, box.y0), *win->PrepareImage, BLRectI((int)box.x0, (int)box.y0, (int)w, (int)h));
            context->end();
        }
        delete win->MosaicImage;
        win->IsMosaicUsePen = false;
        win->MosaicImage = nullptr;
        win->IsDrawing = false;
        context->begin(*win->PrepareImage);
        context->clearAll();
        context->end();
        win->state = win->PreState;
        win->Refresh();
        return true;
    }
    void Mosaic::ShowDragger()
    {
        auto win = WindowBase::Get();
        if (!IsFill)
        {
            win->Refresh();
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
        auto context = win->PaintCtx;
        context->begin(*win->PrepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 4);
        context->end();
        win->Refresh();
    }

    void Mosaic::MouseInDragger(const double& x, const double& y)
    {
        if (!IsFill)
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
    void Mosaic::DragDragger(const double& x, const double& y)
    {
        if (!IsFill) return;
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
                auto win = WindowBase::Get();
                auto x0 = x - win->MouseDownPos.x + box.x0;
                auto y0 = y - win->MouseDownPos.y + box.y0;
                auto x1 = x - win->MouseDownPos.x + box.x1;
                auto y1 = y - win->MouseDownPos.y + box.y1;
                win->MouseDownPos.x = x;
                win->MouseDownPos.y = y;
                if (x0<0 || y0<0 || x1>win->w || y1>win->h) return;
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
