#include "MainWin.h"
#include <memory>

static MainWin* mainWin;


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    Painter::Init();
    painter = Painter::Get();
    initWindowBoxes();
    createWindow();    
    showWindow();
    //InitWindow(painter->x,painter->y,painter->w,painter->h,false);
}
MainWin::~MainWin()
{
    Font::Dispose();
    Painter::Dispose();
}
void MainWin::Init(HINSTANCE hinstance)
{
    mainWin = new MainWin(hinstance);
}
MainWin* MainWin::Get()
{
    return mainWin;
}
void MainWin::Dispose()
{
    delete mainWin;
}