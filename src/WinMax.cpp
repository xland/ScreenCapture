#include "WinMax.h"
#include <memory>
#include <Windows.h>
#include <include/core/SkImage.h>
#include <include/core/SkPixmap.h>
#include <include/core/SkBitmap.h>
#include "Screen.h"
#include "CutMask.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"


WinMax::WinMax()
{

}

WinMax::~WinMax()
{
}

void WinMax::Init()
{
    screen = std::make_unique<Screen>();
    screen->Init();
    x = screen->x;
    y = screen->y;
    w = screen->w;
    h = screen->h;
    cutMask = std::make_unique<CutMask>();
    cutMask->Init();
    toolMain = std::make_unique<ToolMain>();
    toolSub = std::make_unique<ToolSub>();
    InitSurface();
    InitWindow();
}

void WinMax::onPaint()
{
    static auto rowSize = w * sizeof(SkColor);
    auto info = winCanvas->imageInfo();
    winCanvas->writePixels(info, &(screen->screenPix.front()), rowSize, 0, 0);

    canvas->clear(0x00000000);
    auto c = canvas.get();
    for (size_t i = 0; i < paintHandlers.size(); i++)
    {
        paintHandlers[i](c);
    }

    SkPixmap pixmap(info, &canvasPix.front(), rowSize);
    SkBitmap bitmap;
    bitmap.installPixels(pixmap);
    bitmap.setImmutable();
    auto img = bitmap.asImage();
    winCanvas->drawImage(img, 0, 0);
}

