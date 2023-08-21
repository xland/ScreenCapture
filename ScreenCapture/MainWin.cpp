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
    delete desktopImage;
    delete canvasImage;
    delete prepareImage;
    delete mosaicImage;
    delete BottomImage;
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