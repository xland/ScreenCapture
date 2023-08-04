#include "Number.h"
#include <iostream>
#include <string>
#include <Windows.h> 
#include "../Font.h"
#include "../MainWin.h"
namespace Shape {
    static double PI = 3.1415926;
    static unsigned int num = 0;
    Number::Number()
    {
        num += 1;
        if (num > 99) num = 99;
        number = num;
    }
	void Number::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        isTemp = false;
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
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
        auto angle = radint * 180 / PI; //角度
        auto angleSpan = 16.f; //半角
        auto angle1 = (angle + angleSpan) * PI / 180;//弧度
        auto angle2 = (angle - angleSpan) * PI / 180;//弧度
        auto X1 = centerPoint.x + r * cos(angle1);//箭头与圆的交接点1
        auto Y1 = centerPoint.y - r * sin(angle1);
        auto X2 = centerPoint.x + r * cos(angle2);//箭头与圆的交接点2
        auto Y2 = centerPoint.y - r * sin(angle2);
        //auto debug = std::to_string(angle) + "," 
        //    + std::to_string(X1) + "," + std::to_string(Y1) + ","
        //    + std::to_string(x1) + "," + std::to_string(y1) + ",";
        //OutputDebugStringA(debug.c_str());
        //OutputDebugStringA("\r\n");
        
        path.moveTo(arrowPoint.x, arrowPoint.y);
        path.lineTo(X1, Y1);
        path.lineTo(X2, Y2);
        context->setFillStyle(color);
        context->fillPath(path);
        circle.cx = centerPoint.x;
        circle.cy = centerPoint.y;
        circle.r = r;
        if (isFill)
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
        font->setSize(r);
        BLPoint p(0, y2 + r / 3);
        if (number < 10) {
            p.x = x2 - r / 4;
            p.y = y2 + r / 3;
        }
        else
        {
            p.x = x2 - r / 2;
        }
        context->fillUtf8Text(p, *font, std::to_string(number).c_str());
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
	}
    bool Number::EndDraw()
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
        
        context->setFillStyle(color);
        context->fillPath(path);
        if (isFill)
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
        context->begin(*painter->prepareImage);
        context->clearAll();
        context->end();
        painter->isDrawing = false;
        auto win = MainWin::Get();
        win->state = win->preState;
        InvalidateRect(win->hwnd, nullptr, false);
        return true;
    }
    void Number::ShowDragger()
    {
        const BLPoint* points = path.vertexData();
        draggers[0].x0 = points[0].x - draggerSize;
        draggers[0].y0 = points[0].y - draggerSize;
        draggers[0].x1 = points[0].x + draggerSize;
        draggers[0].y1 = points[0].y + draggerSize;
        draggers[1].x0 = circle.cx - draggerSize;
        draggers[1].y0 = circle.cy - draggerSize;
        draggers[1].x1 = circle.cx + draggerSize;
        draggers[1].y1 = circle.cy + draggerSize;

        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->setStrokeStyle(BLRgba32(0, 0, 0));
        context->setStrokeWidth(2);
        context->strokeBoxArray(draggers,2);
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
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
            auto win = MainWin::Get();
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