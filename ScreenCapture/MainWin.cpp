#include "MainWin.h"
#include <Windows.h>


MainWin::MainWin():nu::Window({.frame = false}) 
{
    rect = nu::RectF(GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_CXVIRTUALSCREEN),
        GetSystemMetrics(SM_CYVIRTUALSCREEN));
    scaleFactor = GetScaleFactor();
    SetHasShadow(false);
    SetResizable(false);
    SetBounds(rect);
    SetContentSize(rect.size());   
    container = new nu::Container();
    SetContentView(container);
    container->on_draw.Connect([this](nu::Container* self, nu::Painter* painter, nu::RectF rect) {
        paint(painter);
    });
    Activate();
    //canvas = new nu::Canvas(rect.size(),scaleFactor);
    //
    // Quit when window is closed.
    on_close.Connect([](nu::Window*) {
        nu::MessageLoop::Quit();
    });
}

void MainWin::paint(nu::Painter* painter)
{
    painter->SetFillColor(nu::Color(255, 155, 0, 166));
    painter->FillRect(nu::RectF(100, 100, 200, 200));

    painter->SetFillColor(nu::Color(255, 155, 155, 255));
    painter->FillRect(nu::RectF(200, 200, 200, 200));

    //painter->SetBlendMode(nu::BlendMode::Normal);
    //painter->SetFillColor(nu::Color(0, 0, 0, 0));
    //painter->FillRect(nu::RectF(210, 210, 20, 20));
    painter->ClipRect(nu::RectF(210, 210, 20, 20));


    //painter->BeginPath();
    //painter->ClosePath();
    //painter->Stroke();
}