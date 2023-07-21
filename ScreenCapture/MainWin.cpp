#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    shotScreen();  
    createWindow();
    initScaleFactor();
    showWindow();
    font = new Font();
}
MainWin::~MainWin()
{
    delete bgImage;
    delete canvasImage;
    delete fontIcon;
    delete font;
    DeleteObject(bgHbitmap);
}