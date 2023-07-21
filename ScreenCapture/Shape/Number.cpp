#include "Number.h"
#include <iostream>
#include <string>
#include <Windows.h> 
#include "../Font.h"
namespace Shape {
    static double PI = 3.1415926;
	void Number::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
	{
        
        auto x = x1 - x2;
        auto y = y2 - y1;
        auto r = std::sqrt(x * x + y * y);
        auto radint = std::atan2(y, x);
        auto angle = radint * 180 / PI;
        auto angleSpan = 16.f;
        auto height = r / 2.6;
        auto angle1 = (angle + angleSpan) * PI / 180;
        auto angle2 = (angle - angleSpan) * PI / 180;
        auto X0 = x2 + (r + height) * cos(radint);
        auto Y0 = y2 - (r + height) * sin(radint);
        auto X1 = x2 + r * cos(angle1);
        auto Y1 = y2 - r * sin(angle1);
        auto X2 = x2 + r * cos(angle2);
        auto Y2 = y2 - r * sin(angle2);
        //auto debug = std::to_string(angle) + "," 
        //    + std::to_string(X1) + "," + std::to_string(Y1) + ","
        //    + std::to_string(x1) + "," + std::to_string(y1) + ",";
        //OutputDebugStringA(debug.c_str());
        //OutputDebugStringA("\r\n");
        BLPath path;
        path.moveTo(X0, Y0);
        path.lineTo(X1, Y1);
        path.lineTo(X2, Y2);
        paintCtx->setFillStyle(color);
        paintCtx->fillPath(path);
        BLCircle circle(x2,y2,r);
        if (isFill)
        {
            paintCtx->setFillStyle(color);
            paintCtx->fillCircle(circle);
            paintCtx->setFillStyle(BLRgba32(0xFFFFFFFF));
        }
        else
        {
            paintCtx->setStrokeStyle(color);
            paintCtx->setStrokeWidth(strokeWidth);
            paintCtx->strokeCircle(circle);
            paintCtx->setFillStyle(color);
        }       
        auto font = Font::Get()->fontText;
        font->setSize(r);
        paintCtx->fillUtf8Text(BLPoint(x2-r/4, y2+r/3), *font, "8");
	}
}