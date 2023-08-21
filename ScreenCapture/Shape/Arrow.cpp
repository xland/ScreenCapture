#include "Arrow.h"
#include <math.h>
#include "../MainWin.h"

namespace Shape {

    Arrow::Arrow()
    {
        state = State::arrow;
    }
    Arrow::~Arrow()
    {

    }
	void Arrow::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        isTemp = false;
        auto win = MainWin::Get();
        auto context = win->PaintCtx;
        context->begin(*win->prepareImage);
        context->clearAll();
        context->setStrokeCaps(BL_STROKE_CAP_ROUND);
        if(x1 != -1){
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
        }
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
        win->Refresh();
	}
    bool Arrow::EndDraw()
    {
        auto win = MainWin::Get();
        if (!win->IsDrawing) {
            return true;
        }
        if (draggerIndex != -1) {
            return false;
        }
        auto context = win->PaintCtx;
        context->begin(*win->canvasImage);
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
        context->begin(*win->prepareImage);
        context->clearAll();
        context->end();
        win->IsDrawing = false;
        win->state = win->preState;
        win->Refresh();
        return true;
    }
    void Arrow::ShowDragger()
    {
        auto win = MainWin::Get();
        const BLPoint* points = path.vertexData();
        if (!points)return;
        draggers[0].x0 = points[0].x - draggerSize;
        draggers[0].y0 = points[0].y - draggerSize;
        draggers[0].x1 = points[0].x + draggerSize;
        draggers[0].y1 = points[0].y + draggerSize;

        draggers[1].x0 = points[3].x - draggerSize;
        draggers[1].y0 = points[3].y - draggerSize;
        draggers[1].x1 = points[3].x + draggerSize;
        draggers[1].y1 = points[3].y + draggerSize;

        auto context = win->PaintCtx;
        context->begin(*win->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers, 2);
        context->end();
        win->Refresh();
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