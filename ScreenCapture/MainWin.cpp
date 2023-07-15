#include "MainWin.h"
#include <memory>
#include <blend2d.h>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    shotScreen();  

    BLImage img(w, h, BL_FORMAT_PRGB32);
    BLContext ctx(img);
    BLPath path;
    path.moveTo(0, 0);
    path.lineTo(w, h);
    ctx.setStrokeStyle(BLRgba32(0xFFFFFFFF));
    ctx.strokePath(path);
    ctx.end();
    BLImageData data;
    img.getData(&data);
    unsigned int dataSize = w * h * 4;
    char* tempData = (char*)(data.pixelData);
    for (int x = 0; x < dataSize; x += 4)
    {
        bgPixels[x + 2] = tempData[x]; //red
        bgPixels[x + 1] = tempData[x+1]; //green
        bgPixels[x] = tempData[x + 2]; //blue
    }
    createWindow();
    initScaleFactor();
    showWindow();
}
MainWin::~MainWin()
{    
}