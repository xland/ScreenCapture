#include "Mosaic.h"
#include "../Util.h"
#include "../MainWin.h"

namespace Shape {

    Mosaic::Mosaic()
    {
        bgImgData = new BLImageData();
        canvasImgData = new BLImageData();
    }
    Mosaic::~Mosaic()
    {
        delete bgImgData;
        delete canvasImgData;
    }
    void Mosaic::drawMosaic(BLContext* context)
    {
        BLPointI points[5];
        unsigned int b{ 0 }, g{ 0 }, r{ 0 }, a{ 0 };
        unsigned char* bgData = (unsigned char*)(bgImgData->pixelData);
        unsigned char* canvasData = (unsigned char*)(canvasImgData->pixelData);
        for (int y = (int)box.y0; y < box.y1; y += (int)strokeWidth)
        {
            for (int x = (int)box.x0; x < box.x1; x += (int)strokeWidth)
            {
                //left top point
                points[0].x = x;
                points[0].y = y;
                //right top point
                points[1].y = y;
                if (x + (int)strokeWidth > box.x1) {
                    points[1].x = (int)box.x1;
                    points[4].x = (int)box.x1;
                }
                else
                {
                    points[1].x = x + (int)strokeWidth;
                    points[4].x = x + (int)strokeWidth;
                }
                //center point
                if (x + (int)strokeWidth / 2 > box.x1) {
                    points[2].x = x + (int)((box.x1 - x));  //(int)((box.x1 - x) / 2)
                }
                else
                {
                    points[2].x = x + (int)strokeWidth / 2;
                }
                if (y + (int)strokeWidth / 2 > box.y1) {
                    points[2].y = y + (int)((box.y1 - y)); //(int)((box.y1 - y) / 2)
                }
                else
                {
                    points[2].y = y + (int)strokeWidth / 2;
                }
                //left down point
                points[3].x = x;
                if (y + (int)strokeWidth > box.y1) {
                    points[3].y = box.y1;
                    points[4].y = box.y1;
                }
                else
                {
                    points[3].y = y + (int)strokeWidth;
                    points[4].y = y + (int)strokeWidth;
                }

                for (size_t i = 0; i < 5; i++)
                {
                    auto index = points[i].y * (int)screenW * 4 + points[i].x * 4;
                    if (canvasData[index + 3] == 0) {
                        b += bgData[index];
                        g += bgData[index + 1];
                        r += bgData[index + 2];
                    }
                    else if(canvasData[index + 3] == 255)
                    {
                        b += canvasData[index];
                        g += canvasData[index + 1];
                        r += canvasData[index + 2];
                    }
                    else
                    {
                        double a = (double)canvasData[index + 3] / 255.0f;
                        b += (unsigned int)std::round((double)canvasData[index+0] * a + (double)bgData[index+0] * (1 - a)); 
                        g += (unsigned int)std::round((double)canvasData[index+1] * a + (double)bgData[index+1] * (1 - a)); 
                        r += (unsigned int)std::round((double)canvasData[index+2] * a + (double)bgData[index+2] * (1 - a)); 
                    }
                    //a += bgData[index + 3];
                }
                //5 point colors average
                context->setFillStyle(BLRgba32(r / 5, g / 5, b / 5, 255));
                context->fillBox(x, y, points[4].x, points[4].y);
                b = g = r = a = 0;
            }
        }
    }
	void Mosaic::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        isTemp = false;
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();        
        SetBoxByPos(box,x1, y1,x2, y2);//todo 以不同的起始坐标画这个矩形的时候，马赛克出现的效果不一样
        drawMosaic(context);
        context->setStrokeStyle(BLRgba32(0,0,0));
        context->setStrokeWidth(1);
        context->strokeBox(box);
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
	}

    bool Mosaic::EndDraw()
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
        drawMosaic(context);
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
    void Mosaic::ShowDragger()
    {
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

    void Mosaic::MouseInDragger(const double& x, const double& y)
    {
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
