#include "Arrow.h"
#include <math.h>

namespace Shape {

    Arrow::Arrow()
    {
        //path.moveTo(-1.0f, -1.0f);
        //for (size_t i = 0; i < 6; i++)
        //{
        //    path.lineTo(-1.0f, -1.0f);
        //}
    }
    Arrow::~Arrow()
    {

    }
	void Arrow::Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2)
	{
        path.clear();
        path.moveTo(x1, y1);
        path.lineTo(x2, y2);
        //double height = 26.0;
        //double width = 22.0;
        //auto x = x2 - x1;
        //auto y = y2 - y1;
        //auto z = sqrt(x * x + y * y);
        //auto sin = y / z;
        //auto cos = x / z;
        //// △底边的中点
        //double centerX = x2 - height * cos;
        //double centerY = y2 + height * sin;
        //double tempA = width / 4 * sin;
        //double tempB = width / 4 * cos;
        //// △ 左下的顶点与底边中点之间中间位置的点
        //double X1 = centerX - tempA;
        //double Y1 = centerY - tempB;
        //path.lineTo(X1, Y1);
        //// △ 左下的顶点
        //double X2 = x1 - tempA;
        //double Y2 = y1 - tempB;
        //path.lineTo(X2, Y2);
        //// △ 上部顶点，也就是箭头终点
        //path.lineTo(x2, y2);
        //// △ 右下顶点
        //tempA = width / 2 * sin;
        //tempB = width / 2 * cos;
        //double X3 = centerX + tempA;
        //double Y3 = centerY + tempB;
        //path.lineTo(X3, Y3);
        //// △ 右下的顶点与底边中点之间中间位置的点
        //double X4 = centerX + tempA / 2;
        //double Y4 = centerY + tempB / 2;
        //path.lineTo(X4, Y4);
        //path.lineTo(x1, y1);
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillPath(path);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth);
            context->strokePath(path);
        }
	}
}