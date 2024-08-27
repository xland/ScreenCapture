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
    InitSurface();
    InitWindow();
}

WinMax::~WinMax()
{
}

void WinMax::onPaint()
{
    auto screen = Screen::Get();
    static auto rowSize = w * sizeof(SkColor);
    auto info = winCanvas->imageInfo();
    winCanvas->writePixels(info, &screen->screenPix.front(), rowSize, 0, 0);

    canvas->clear(0x00000000);
    auto c = canvas.get();
    cutMask->Paint(c);
    toolMain->Paint(c);
    toolSub->Paint(c);

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
    toolMain->onLeftBtnDown(x, y);
    toolSub->onLeftBtnDown(x, y);
}

void WinMax::onLeftBtnUp(const int& x, const int& y)
{
    cutMask->onLeftBtnUp(x, y);
    toolMain->onLeftBtnUp(x, y);
    toolSub->onLeftBtnUp(x, y);
}

void WinMax::onMouseMove(const int& x, const int& y)
{
    cutMask->onMouseMove(x, y);
    toolMain->onMouseMove(x, y);
    toolSub->onMouseMove(x, y);
}

void WinMax::onMouseDrag(const int& x, const int& y)
{
    cutMask->onMouseDrag(x, y);
    toolMain->onMouseDrag(x, y);
    toolSub->onMouseDrag(x, y);
}

void WinMax::onCustomMsg(const EventType& type, const uint32_t& msg)
{
    toolMain->onCustomMsg(type, msg);
    toolSub->onCustomMsg(type, msg);
}
