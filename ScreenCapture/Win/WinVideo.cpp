#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinVideo.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "History.h"
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

void WinVideo::startMp4(bool useSpeaker, bool useMic)
{
    setMouseTransparent(true);
    auto videoTempPath = App::getDataPath();
    mp4Param = std::make_unique<WinVideoMp4::DESKTOPCAPTUREPARAMS>();
    mp4Param->VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    mp4Param->rx = { (long)(x + cutMask->maskRect.left), (long)(y+ cutMask->maskRect.top), (long)(cutMask->maskRect.right- cutMask->maskRect.left), (long)(cutMask->maskRect.bottom - cutMask->maskRect.top) };
    mp4Param->f = videoTempPath.append(Util::createFileName(L"mp4")).wstring();
    mp4Param->EndMS = 0;
    mp4Param->fps = 30;
    mp4Param->vbrm = 2;
    mp4Param->vbrq = 50;
    mp4Param->Qu = 50;
    mp4Param->MustEnd = false;
    WinVideoMp4::setAudio(mp4Param.get(), useSpeaker, useMic);
    captureThread = std::jthread([this](std::stop_token st) {
        HRESULT hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) return;
        hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) return;
        WinVideoMp4::DesktopCapture(*(mp4Param.get()));
        CoUninitialize();
        MFShutdown();
    });
}

void WinVideo::startGif()
{
    setMouseTransparent(true);
    auto videoTempPath = App::getDataPath();
    gifParam = std::make_unique<WinVideoGif::GifParam>();
    gifParam->x = (int)(this->x + cutMask->maskRect.left);
    gifParam->y = (int)(this->y + cutMask->maskRect.top);
    gifParam->w = (int)(cutMask->maskRect.right - cutMask->maskRect.left);
    gifParam->h = (int)(cutMask->maskRect.bottom - cutMask->maskRect.top);
    gifParam->path = videoTempPath.append(Util::createFileName(L"gif")).wstring();
    captureThread = std::jthread([this](std::stop_token st) {
        WinVideoGif::createGif(gifParam.get());
    });
}

std::wstring WinVideo::stop()
{
    std::wstring filePath;
    if (mp4Param.get()) {
        mp4Param->MustEnd = true;
        filePath = mp4Param->f;
    }
    if (gifParam.get()) {
        gifParam->isFinish = true;
        filePath = gifParam->path;
    }
    if (captureThread.joinable()) {
        captureThread.join();
    }
    return filePath;
}

LRESULT WinVideo::onHitTest(WPARAM wParam, LPARAM lParam)
{
    if (isMouseTransparent || isFinishCutMask || mp4Param || gifParam) {
        return HTTRANSPARENT;
    }
    return HTCLIENT;
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
        setMouseTransparent(true);
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

void WinVideo::drawCursor(HDC hMemDC, const int& width, const int& height) {
    CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
    GetCursorInfo(&cursorInfo);
    if (cursorInfo.flags == CURSOR_SHOWING) {
        ICONINFO iconInfo;
        GetIconInfo(cursorInfo.hCursor, &iconInfo);
        int localX = cursorInfo.ptScreenPos.x - x - iconInfo.xHotspot;
        int localY = cursorInfo.ptScreenPos.y - y - iconInfo.yHotspot;
        if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
        if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
        if (localX >= 0 && localX < width && localY >= 0 && localY < height) {
            DrawIconEx(hMemDC, localX, localY, cursorInfo.hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_DEFAULTSIZE);
        }
    }
}

void WinVideo::setMouseTransparent(bool transparent)
{
    isMouseTransparent = transparent;

    if (!hwnd) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    if (transparent) {
        exStyle |= WS_EX_LAYERED;
        exStyle |= WS_EX_TRANSPARENT;

        if (GetCapture() == hwnd) {
            ReleaseCapture();
        }

        isMouseDown = false;
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }
    else {
        exStyle &= ~WS_EX_TRANSPARENT;
        exStyle &= ~WS_EX_LAYERED;
    }

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOZORDER |
        SWP_NOACTIVATE |
        SWP_FRAMECHANGED
    );
}