#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    shotScreen();  
    BLContext ctx(*bgImage);
    BLPath path;
    path.moveTo(0, 0);
    path.lineTo(w, h);
    path.moveTo(w, 0);
    path.lineTo(0, h);
    ctx.setStrokeStyle(BLRgba32(0, 0, 255));
    ctx.setStrokeWidth(12.6);
    ctx.strokePath(path);
    ctx.end();
    createWindow();
    initScaleFactor();
    d2DCreateFactory();
    showWindow();
}
MainWin::~MainWin()
{
    delete bgImage;
    d2DImage->Release();
    render->Release();
    factory->Release();
}