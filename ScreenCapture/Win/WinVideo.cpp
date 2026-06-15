#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinVideo.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "History.h"
#include "gifski.h"
#include "WinVideoMp4.hpp"
#include "Tool/ToolVideo.h"

std::unique_ptr<WinVideo> winVideo;

WinVideo::WinVideo(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{

}

WinVideo::~WinVideo()
{

}

void WinVideo::init()
{
    WinVideo::release();
    auto [x, y, w, h] = Util::getDesktopInfo();
    winVideo = std::make_unique<WinVideo>(x, y, w, h);
    winVideo->createWindow();    
    winVideo->cutMask = std::make_unique<WinCutMask>(winVideo.get());
    winVideo->show();
}

void WinVideo::release()
{
    if (winVideo.get()) {
        winVideo->close();
        winVideo.reset();
    }
}

void WinVideo::onPaint()
{
    render->Clear(0);
    cutMask->paint();
}

void WinVideo::onMouseMove(const int& x, const int& y) 
{
    if (isFinishCutMask)  return;
    cutMask->highlight(x, y);
}
void WinVideo::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (isFinishCutMask)  return;
    cutMask->makeRect(x, y);
}
void WinVideo::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        App::exit(2);
        return;
    }
    if (isFinishCutMask)  return;
    cutMask->startMakeRect(x, y);
}
void WinVideo::onMouseUp(const int& x, const int& y)
{
    if (!isFinishCutMask) {
        isFinishCutMask = true;
        makeTool();
        return;
    }
}

void WinVideo::makeTool()
{
    auto btnSize{ 32.f * dpi };
    POINT pos{ 0,0 };
    pos.x = cutMask->maskRect.left;
    pos.y = cutMask->maskRect.bottom + cutMask->strokeWidth + 2*dpi; //todo 
    ClientToScreen(hwnd, &pos);
    tool = std::make_unique<ToolVideo>(pos.x, pos.y, 232.f*dpi, btnSize, this);
    tool->createWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    tool->show();
}
