#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinVideo.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "History.h"
#include "gifski.h"
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

void WinVideo::start()
{
    dp.VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    dp.rx = { (long)(x + cutMask->maskRect.left), (long)(y+ cutMask->maskRect.top),
        (long)(cutMask->maskRect.right- cutMask->maskRect.left),
        (long)(cutMask->maskRect.bottom - cutMask->maskRect.top) };
    dp.f = L"desktopVideo.mp4";
    //for (auto& id : ids)
    //{
    //    dp.AudioFrom.push_back({ id, {0,1} });
    //}
    dp.EndMS = 0;
    dp.fps = 30;
    dp.vbrm = 2;
    dp.vbrq = 50;
    dp.Qu = 50;
    dp.MustEnd = false;

    captureThread = std::jthread([this](std::stop_token st) {
        HRESULT hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) return;
        hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) return;
        WinVideoMp4::DesktopCapture(dp);
        CoUninitialize();
        MFShutdown();
    });
}

void WinVideo::stop()
{
    dp.MustEnd = true;
    captureThread.join();
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
        //todo
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
