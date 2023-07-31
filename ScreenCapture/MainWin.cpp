#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    Painter::Init();
    painter = Painter::Get();
    createWindow();
    initScaleFactor();
    showWindow();
}
MainWin::~MainWin()
{
    Font::Dispose();
    Painter::Dispose();
}