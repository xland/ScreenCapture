#include "MainWin.h"
#include <memory>

static MainWin* mainWin;


MainWin::MainWin()
{
    initWindowBoxes();
    //createWindow();    
    //showWindow();
    initLayerImg();
    InitWindow(false);
}
MainWin::~MainWin()
{
    Font::Dispose();
    delete PaintCtx;
    delete DesktopImage;
    delete CanvasImage;
    delete PrepareImage;
    delete MosaicImage;
    delete bottomImage;
}
void MainWin::Init()
{
    mainWin = new MainWin();
}
MainWin* MainWin::Get()
{
    return mainWin;
}
void MainWin::Dispose()
{
    delete mainWin;
}