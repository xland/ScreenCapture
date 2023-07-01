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
}