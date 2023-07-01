#include "MainWin.h"
#include <memory>



MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    shotScreen();  
    createWindow();
    initScaleFactor();
    showWindow();
    initCanvas();
    createDeviceRes();
    paint();
}
MainWin::~MainWin()
{
    
    //bgImg->Release();
    //d2dBitmap->Release();
    //d2d1Device->Release();
    //d2dfactory1->Release();
    //d2dFactory->Release();
    // 
    // 
    // 
    //brush->Release();
    //maskBrush->Release();
    //bgImg->Release();
    //maskLayer->Release();
    //render->Release();
    //factory->Release();
    
}