#include "Number.h"
#include <numbers>
#include <iostream>
#include <string>
#include <Windows.h> 
#include "../Font.h"
#include "../WindowBase.h"
namespace Shape {
    static int num = 1;
    Number::Number()
    {
        state = State::number;
    }
    Number::~Number() {
        if (num > number) {
            num = number;
        }
    }
    void Number::SetNumber()
    {
        number = num;
        num += 1;
    }
	void Number::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto win = WindowBase::Get();
        auto context = win->PaintCtx;
        context->begin(*win->PrepareImage);
        context->clearAll();        

        if(x1 != -1){
            path.clear();
            BLPoint arrowPoint, centerPoint;
            arrowPoint.x = x1; //箭头顶点
            arrowPoint.y = y1;
            centerPoint.x = x2; //圆心
            centerPoint.y = y2;
            auto x = x1 - x2;
            auto y = y2 - y1;
            auto r = std::sqrt(x * x + y * y);//圆心到箭头顶点的长度
            auto height = r / 2.6; //箭头高度
            r = r - height;//半径
            auto radint = std::atan2(y, x); //反正切
            auto angle = radint * 180 / std::numbers::pi; //角度
            auto angleSpan = 16.f; //半角
            auto angle1 = (angle + angleSpan) * std::numbers::pi / 180;//弧度
            auto angle2 = (angle - angleSpan) * std::numbers::pi / 180;//弧度
            auto X1 = centerPoint.x + r * cos(angle1);//箭头与圆的交接点1
            auto Y1 = centerPoint.y - r * sin(angle1);
            auto X2 = centerPoint.x + r * cos(angle2);//箭头与圆的交接点2
            auto Y2 = centerPoint.y - r * sin(angle2);        
            path.moveTo(arrowPoint.x, arrowPoint.y);
            path.lineTo(X1, Y1);
            path.lineTo(X2, Y2);

            circle.cx = centerPoint.x;
            circle.cy = centerPoint.y;
            circle.r = r;
        }
        context->setFillStyle(color);
        context->fillPath(path);
        if (IsFill)
        {
            context->setFillStyle(color);
            context->fillCircle(circle);
            context->setFillStyle(BLRgba32(0xFFFFFFFF));
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokeCircle(circle);
            context->setFillStyle(color);
        }       
        auto font = Font::Get()->fontText;
        font->setSize(circle.r);
        BLPoint p(0, circle.cy + circle.r / 3);
        if (number < 10) {
            p.x = circle.cx - circle.r / 4;
            p.y = circle.cy + circle.r / 3;
        }
        else
        {
            p.x = circle.cx - circle.r / 2;
        }
        context->fillUtf8Text(p, *font, std::to_string(number).c_str());
        context->end();
        win->Refresh();
	}
    bool Number::EndDraw()
    {
        auto win = WindowBase::Get();
        if (!win->IsDrawing) {
            return true;
        }
        if (draggerIndex != -1) {
            return false;
        }
        auto context = win->PaintCtx;
        context->begin(*win->CanvasImage);
        
        context->setFillStyle(color);
        context->fillPath(path);
        if (IsFill)
        {
            context->setFillStyle(color);
            context->fillCircle(circle);
            context->setFillStyle(BLRgba32(0xFFFFFFFF));
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokeCircle(circle);
            context->setFillStyle(color);
        }
        auto font = Font::Get()->fontText;
        font->setSize(circle.r);
        
        BLPoint p(0, circle.cy + circle.r / 3);
        if (number < 10) {
            p.x = circle.cx - circle.r / 4;
            p.y = circle.cy + circle.r / 3;
        }
        else
        {
            p.x = circle.cx - circle.r / 2;
        }
        context->fillUtf8Text(p, *font, std::to_string(number).c_str());

        context->end();
        context->begin(*win->PrepareImage);
        context->clearAll();
        context->end();
        win->IsDrawing = false;
        win->state = win->PreState;
        win->Refresh();
        return true;
    }
    void Number::ShowDragger()
    {
        const BLPoint* points = path.vertexData();
        if (!points) return;
        draggers[0].x0 = points[0].x - draggerSize;
        draggers[0].y0 = points[0].y - draggerSize;
        draggers[0].x1 = points[0].x + draggerSize;
        draggers[0].y1 = points[0].y + draggerSize;
        draggers[1].x0 = circle.cx - draggerSize;
        draggers[1].y0 = circle.cy - draggerSize;
        draggers[1].x1 = circle.cx + draggerSize;
        draggers[1].y1 = circle.cy + draggerSize;
        auto win = WindowBase::Get();
        auto context = win->PaintCtx;
        context->begin(*win->PrepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers,2);
        context->end();
        win->Refresh();
    }
    void Number::MouseInDragger(const double& x, const double& y)
    {
        if (draggers[0].contains(x, y)) {
            tempDraggerX = circle.cx;
            tempDraggerY = circle.cy;
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
    void Number::DragDragger(const double& x, const double& y)
    {
        if (draggerIndex == 0) {
            Draw(x, y, tempDraggerX, tempDraggerY);
        }
        else if (draggerIndex == 1) {
            auto win = WindowBase::Get();
            auto xSpan = x - win->MouseDownPos.x;
            auto ySpan = y - win->MouseDownPos.y;
            tempDraggerX += xSpan;
            tempDraggerY += ySpan;
            auto X = circle.cx + xSpan;
            auto Y = circle.cy + ySpan;
            Draw(tempDraggerX, tempDraggerY, X, Y);
            win->MouseDownPos.x = x;
            win->MouseDownPos.y = y;
        }
    }
}