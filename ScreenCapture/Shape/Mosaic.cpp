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
        SetBoxByPos(box,x1, y1,x2, y2);
        for (size_t y = box.y0; y < box.y1; y += strokeWidth)
        {
            for (size_t x = box.x0; x < box.x1; x+=strokeWidth)
            {
                auto index = y * (int)screenW * 4 + x * 4;
                char* data = (char*)(bgImgData->pixelData);
                auto r1 = data[index];
                auto g1 = data[index+1];
                auto b1 = data[index+2];
                auto a1 = data[index+3];
                paintCtx->setFillStyle(BLRgba32(r1, g1, b1, a1));
                paintCtx->fillBox(x, y, x + strokeWidth, y + strokeWidth);


                //index += strokeWidth * 4;
                //auto r2 = data[index];
                //auto g2 = data[index + 1];
                //auto b2 = data[index + 2];
                //auto a2 = data[index + 3];

                //index += y*((strokeWidth/2)*4);
                //auto r2 = data[index];
                //auto g2 = data[index + 1];
                //auto b2 = data[index + 2];
                //auto a2 = data[index + 3];
            }
        }
	}
}
