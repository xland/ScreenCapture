#include "MainWin.h"
#include <memory>



MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance},
factory{nullptr},hwnd{nullptr}
{
    shotScreen();  
    createWindow();
    initScaleFactor();
    initCanvas();
    showWindow();
}
MainWin::~MainWin()
{
    delete[] bgPixels;
    brush->Release();
    maskBrush->Release();
    bgImg->Release();
    maskLayer->Release();
    render->Release();
    factory->Release();
    
}