#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    shotScreen();  
    createWindow();
    initScaleFactor();
    showWindow();
}
MainWin::~MainWin()
{
    delete bgImage;
    delete canvasImage;
    DeleteObject(bgHbitmap);
    //d2DImage->Release();
    //render->Release();
    //factory->Release();
}