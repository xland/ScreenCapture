#include "Mosaic.h"
#include "../Util.h"

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
	void Mosaic::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
	{
        BLPointI points[5];
        unsigned int b{ 0 }, g{ 0 }, r{ 0 }, a{ 0 };
        unsigned char* data = (unsigned char*)(bgImgData->pixelData);
        SetBoxByPos(box,x1, y1,x2, y2);
        for (size_t y = box.y0; y < box.y1; y += strokeWidth)
        {
            for (size_t x = box.x0; x < box.x1; x+=strokeWidth)
            {
                //left top point
                points[0].x = x;
                points[0].y = y;
                //right top point
                points[1].y = y;
                if (x + strokeWidth > box.x1) {
                    points[1].x = box.x1;
                }
                else
                {
                    points[1].x = x+strokeWidth;
                }
                //center point
                if (x + (int)strokeWidth/2 > box.x1) {
                    points[2].x = x+(int)((box.x1 -x)/2);
                }
                else
                {
                    points[2].x = x + (int)strokeWidth / 2;
                }
                if (y + (int)strokeWidth / 2 > box.y1) {
                    points[2].y = y + (int)((box.y1 - y) / 2);
                }
                else
                {
                    points[2].y = y + (int)strokeWidth / 2;
                }
                //left down point
                points[3].x = x;
                if (y + (int)strokeWidth > box.y1) {
                    points[3].y = box.y1;
                }
                else
                {
                    points[3].y = y + (int)strokeWidth;
                }
                //right down point
                if (x + (int)strokeWidth > box.x1) {
                    points[4].x = box.x1;
                }
                else
                {
                    points[4].x = x + (int)strokeWidth;
                }
                if (y + (int)strokeWidth > box.y1) {
                    points[4].y = box.y1;
                }
                else
                {
                    points[4].y = y + (int)strokeWidth;
                }
                
                for (size_t i = 0; i < 5; i++)
                {
                    auto index = points[i].y * (int)screenW * 4 + points[i].x * 4;
                    b += data[index];
                    g += data[index + 1];
                    r += data[index + 2];
                    a += data[index + 3];
                }
                //5 point colors average
                paintCtx->setFillStyle(BLRgba32(r/5, g/5, b/5, a/5));
                paintCtx->fillBox(x, y, x + strokeWidth, y + strokeWidth);
                b = g = r = a = 0;
            }
        }
	}
}
