#include "Arrow.h"
#include <math.h>
#include "../MainWin.h"

namespace Shape {

    Arrow::Arrow()
    {
    }
    Arrow::~Arrow()
    {

    }
	void Arrow::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        isTemp = false;
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        context->setStrokeCaps(BL_STROKE_CAP_ROUND);
        path.clear();
        path.moveTo(x2, y2);
        //path.lineTo(x2, y2);
        double height = 52.0;
        double width = 52.0;
        auto x = x1 - x2;
        auto y = y2 - y1;
        auto z = sqrt(x * x + y * y);
        auto sin = y / z;
        auto cos = x / z;
        // △底边的中点
        double centerX = x1 - height * cos;
        double centerY = y1 + height * sin;
        double tempA = width / 4 * sin;
        double tempB = width / 4 * cos;
        // △ 左下的顶点与底边中点之间中间位置的点
        double X1 = centerX - tempA;
        double Y1 = centerY - tempB;
        path.lineTo(X1, Y1);
        // △ 左下的顶点
        double X2 = X1 - tempA;
        double Y2 = Y1 - tempB;
        path.lineTo(X2, Y2);
        // △ 上部顶点，也就是箭头终点
        path.lineTo(x1, y1);
        // △ 右下顶点
        tempA = width / 2 * sin;
        tempB = width / 2 * cos;
        double X3 = centerX + tempA;
        double Y3 = centerY + tempB;
        path.lineTo(X3, Y3);
        // △ 右下的顶点与底边中点之间中间位置的点
        double X4 = centerX + tempA / 2;
        double Y4 = centerY + tempB / 2;
        path.lineTo(X4, Y4);
        path.lineTo(x2, y2);
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillPath(path);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth-1);
            context->strokePath(path);
        }
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
	}
    bool Arrow::EndDraw()
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
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillPath(path);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth - 1);
            context->strokePath(path);
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
    void Arrow::ShowDragger()
    {
        const BLPoint* points = path.vertexData();
        draggers[0].x0 = points[0].x - draggerSize;
        draggers[0].y0 = points[0].y - draggerSize;
        draggers[0].x1 = points[0].x + draggerSize;
        draggers[0].y1 = points[0].y + draggerSize;

        draggers[1].x0 = points[3].x - draggerSize;
        draggers[1].y0 = points[3].y - draggerSize;
        draggers[1].x1 = points[3].x + draggerSize;
        draggers[1].y1 = points[3].y + draggerSize;

        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 2);
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
    }
    void Arrow::MouseInDragger(const double& x, const double& y)
    {
        if (draggers[0].contains(x, y)) {
            const BLPoint* points = path.vertexData();
            tempDraggerX = points[3].x;
            tempDraggerY = points[3].y;
            draggerIndex = 0;
            ChangeCursor(IDC_SIZEALL);
        }
        else if (draggers[1].contains(x, y)) {
            const BLPoint* points = path.vertexData();
            tempDraggerX = points[0].x;
            tempDraggerY = points[0].y;
            draggerIndex = 1;
            ChangeCursor(IDC_SIZEALL);
        }
        else
        {
            draggerIndex = -1;
            ChangeCursor(IDC_CROSS);
        }        
    }
    void Arrow::DragDragger(const double& x, const double& y)
    {
        if (draggerIndex == 0) {
            Draw(tempDraggerX, tempDraggerY, x, y);
        }
        else if (draggerIndex == 1) {
            Draw(x, y, tempDraggerX, tempDraggerY);
        }
    }
}