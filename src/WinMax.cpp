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

namespace {
	std::shared_ptr<WinMax> winMax;
}

WinMax::WinMax():cutMask{new CutMask()},
    toolMain{ new ToolMain() }, 
    toolSub{ new ToolSub() }
{
    auto screen = Screen::Get();
    screen->Prepare(this);
    cutMask->EnumWinRects();
    initSurface();
    initWindow();
}

WinMax::~WinMax()
{
}

WinMax* WinMax::Get()
{
    return winMax.get();
}

void WinMax::Init()
{
	auto ptr = new WinMax();
	winMax = std::shared_ptr<WinMax>(ptr);
}

void WinMax::onPaint()
{
    auto screen = Screen::Get();
    auto rowSize = w * sizeof(SkColor);
    auto info = winCanvas->imageInfo();
    winCanvas->writePixels(info, &screen->screenPix.front(), rowSize, 0, 0);
    canvas->clear(0x00000000);
    auto c = canvas.get();
    cutMask->Paint(c);
    ToolMain::Get()->OnPaint(canvas);
    ToolSub::Get()->OnPaint(canvas);

    SkPixmap pixmap(info, &canvasPix.front(), rowSize);
    SkBitmap bitmap;
    bitmap.installPixels(pixmap);
    bitmap.setImmutable();
    auto img = bitmap.asImage();
    winCanvas->drawImage(img, 0, 0);
}

void WinMax::onLeftBtnDown(const int& x, const int& y)
{
    cutMask->onLeftBtnDown(x, y);
}

void WinMax::onLeftBtnUp(const int& x, const int& y)
{
    cutMask->onLeftBtnUp(x, y);
}

void WinMax::onMouseMove(const int& x, const int& y)
{
    cutMask->onMouseMove(x, y);
}

void WinMax::onMouseDrag(const int& x, const int& y)
{
    cutMask->onMouseDrag(x, y);
}
