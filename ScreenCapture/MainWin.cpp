#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    painter = new Painter();
    createWindow();
    initScaleFactor();
    showWindow();
}
MainWin::~MainWin()
{
    delete Font::Get();
}