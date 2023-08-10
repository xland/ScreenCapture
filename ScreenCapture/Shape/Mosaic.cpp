#include "Mosaic.h"
#include "../Util.h"
#include "../MainWin.h"

namespace Shape {

    Mosaic::Mosaic()
    {
        state = State::mosaic;
        bgImgData = new BLImageData();
        canvasImgData = new BLImageData();
    }
    Mosaic::~Mosaic()
    {
        delete bgImgData;
        delete canvasImgData;
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
    void Mosaic::setSqureColor(BLPointI* points, unsigned char* bgData, unsigned char* canvasData)
    {
        unsigned int b{ 0 }, g{ 0 }, r{ 0 };
        auto totalSize = screenW * 4 * screenH;
        for (size_t i = 0; i < 5; i++)
        {
            auto index = points[i].y * (int)screenW * 4 + points[i].x * 4;
            if (index > totalSize) {
                break;
            }
            if (canvasData[index + 3] == 0) {
                b += bgData[index];
                g += bgData[index + 1];
                r += bgData[index + 2];
            }
            else if (canvasData[index + 3] == 255)
            {
                b += canvasData[index];
                g += canvasData[index + 1];
                r += canvasData[index + 2];
            }
            else
            {
                double a = (double)canvasData[index + 3] / 255.0f;
                b += (unsigned int)std::round((double)canvasData[index + 0] * a + (double)bgData[index + 0] * (1 - a));
                g += (unsigned int)std::round((double)canvasData[index + 1] * a + (double)bgData[index + 1] * (1 - a));
                r += (unsigned int)std::round((double)canvasData[index + 2] * a + (double)bgData[index + 2] * (1 - a));
            }
        }
        //5 point colors average
        Painter::Get()->paintCtx->setFillStyle(BLRgba32(r / 5, g / 5, b / 5, 255));
    }
    void Mosaic::drawMosaic()
    {
        BLPointI points[5];        
        unsigned char* bgData = (unsigned char*)(bgImgData->pixelData);
        unsigned char* canvasData = (unsigned char*)(canvasImgData->pixelData);
        int y0 = ((int)box.y0 / strokeWidth) * strokeWidth;
        int x0 = ((int)box.x0 / strokeWidth) * strokeWidth;
        int y1 = ((int)box.y1 / strokeWidth) * strokeWidth;
        int x1 = ((int)box.x1 / strokeWidth) * strokeWidth;

        for (int y = y0; y < y1; y += strokeWidth)
        {
            for (int x = x0; x < x1; x += strokeWidth)
            {
                setSamplingPoints(points, x, y);
                setSqureColor(points, bgData, canvasData);
                auto startX = x == x0 ? box.x0 : x;
                auto startY = y == y0 ? box.y0 : y;
                auto endX = points[4].x;
                auto endY = points[4].y;
                Painter::Get()->paintCtx->fillBox(startX, startY, endX, endY);
                if (box.x1 - endX < strokeWidth) {
                    setSamplingPoints(points, points[1].x, points[1].y);
                    setSqureColor(points, bgData, canvasData);
                    startX = points[0].x < box.x0 ? box.x0 : points[0].x;
                    startY = points[0].y < box.y0 ? box.y0 : points[0].y;
                    Painter::Get()->paintCtx->fillBox(startX, startY, box.x1,box.y1);
                }
                if (box.y1 - endY < strokeWidth) {
                    setSamplingPoints(points, points[3].x, points[3].y);
                    setSqureColor(points, bgData, canvasData);
                    startX = points[0].x < box.x0 ? box.x0 : points[0].x;
                    startY = points[0].y < box.y0 ? box.y0 : points[0].y;
                    Painter::Get()->paintCtx->fillBox(startX, startY, box.x1, box.y1);
                }
            }
        }
    }
	void Mosaic::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        isTemp = false;
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();        
        SetBoxByPos(box,x1, y1,x2, y2);
        drawMosaic();
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
        drawMosaic();
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
