#include "MainWin.h"
#include <memory>



MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance},
    direct2dFactory{nullptr},hwnd{nullptr}
{
    shotScreen();  
    createWindow();
    initScaleFactor();
    initCanvas();
    showWindow();
}
MainWin::~MainWin()
{
    brush->Release();
    maskBrush->Release();
    bgImg->Release();
    render->Release();
    direct2dFactory->Release();
    
}