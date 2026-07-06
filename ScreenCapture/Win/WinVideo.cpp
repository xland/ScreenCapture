#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinVideo.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "History.h"
#include "Tool/ToolVideo.h"
#include "Setting.h"

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
    App::initDevice();
    auto [x, y, w, h] = Util::getDesktopInfo();
    winVideo = std::make_unique<WinVideo>(x, y, w, h);
    winVideo->createWindow(WS_EX_TOPMOST);
    winVideo->cutMask = std::make_unique<WinCutMask>(winVideo.get());
    winVideo->show();
}

void WinVideo::release()
{
    if (winVideo.get()) {
		winVideo->stop();
        winVideo->close();
        winVideo.reset();
        if (App::getArg(L"auto-quit") == L"true") {
            App::exit(0);
        }
        else {
            App::disposeDeviceIfIdle();
        }
    }
}

void WinVideo::stopIfRecording()
{
    if (!winVideo.get()) return;
    if (!winVideo->mp4Param && !winVideo->gifParam) return;
    //正在录制：先停止编码线程，避免退出时线程与设备卡死
    winVideo->stop();
}

void WinVideo::startMp4(bool useSpeaker, bool useMic)
{
    setMouseTransparent(true);
    auto videoTempPath = Setting::getDataPath();
    mp4Param = std::make_unique<WinVideoMp4::DESKTOPCAPTUREPARAMS>();
    mp4Param->VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    mp4Param->rx = { (long)(x + cutMask->maskRect.left),
    (long)(y + cutMask->maskRect.top),
    (long)(x + cutMask->maskRect.right),
    (long)(y + cutMask->maskRect.bottom) 
    };
    mp4Param->f = videoTempPath.append(L"temp.mp4").wstring();
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
    auto videoTempPath = Setting::getDataPath();
    gifParam = std::make_unique<WinVideoGif::GifParam>();
    gifParam->x = (int)(this->x + cutMask->maskRect.left);
    gifParam->y = (int)(this->y + cutMask->maskRect.top);
    gifParam->w = (int)(cutMask->maskRect.right - cutMask->maskRect.left);
    gifParam->h = (int)(cutMask->maskRect.bottom - cutMask->maskRect.top);
    gifParam->path = videoTempPath.append(L"temp.gif").wstring();
    captureThread = std::jthread([this](std::stop_token st) {
        WinVideoGif::createGif(gifParam.get());
    });
}

std::wstring WinVideo::stop()
{
    hide();
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
        WinVideo::release();
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

void WinVideo::onKeyDown(const UINT& key)
{
    if (key == VK_ESCAPE) {
        if (tool.get())tool->close();
        WinVideo::release();
    }
}

void WinVideo::makeTool()
{
    const float btnSize = 32.f * dpi;
    const int toolW = (int)(232.f * dpi + 0.5f);
    const int toolH = (int)(btnSize + 0.5f);

    // maskRect 是 WinVideo 客户区坐标，换算到屏幕坐标
    const int maskLeftScr = x + (int)cutMask->maskRect.left;
    const int maskTopScr = y + (int)cutMask->maskRect.top;
    const int maskRightScr = x + (int)cutMask->maskRect.right;
    const int maskBottomScr = y + (int)cutMask->maskRect.bottom;

    // 用框选区域所在显示器的工作区判断上/下方是否有足够空间
    RECT maskScrRect{ maskLeftScr, maskTopScr, maskRightScr, maskBottomScr };
    HMONITOR hMon = MonitorFromRect(&maskScrRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(MONITORINFO) };
    GetMonitorInfo(hMon, &mi);
    const int workLeft = mi.rcWork.left;
    const int workTop = mi.rcWork.top;
    const int workRight = mi.rcWork.right;
    const int workBottom = mi.rcWork.bottom;

    const int gap = (int)(cutMask->strokeWidth + 2.f * dpi + 0.5f); // 与框选边框的间距
    const int neededBelow = gap + toolH;
    const int neededAbove = gap + toolH;

    const bool fitBelow = (maskBottomScr + neededBelow) <= workBottom;
    const bool fitAbove = (maskTopScr - neededAbove) >= workTop;

    // ToolVideo 右侧与框选区域右侧对齐
    int toolX = maskRightScr - toolW;
    int toolY = 0;

    if (fitBelow) {
        // 右下方
        toolY = maskBottomScr + gap;
    }
    else if (fitAbove) {
        // 右上方
        toolY = maskTopScr - gap - toolH;
    }
    else {
        // 叠加在框选区域右下方内部，与右/底各留 3*dpi
        const int overlapPad = (int)(3.f * dpi + 0.5f);
        toolX = maskRightScr - toolW - overlapPad;
        toolY = maskBottomScr - toolH - overlapPad;
    }

    // 兜底：不越出所在显示器工作区
    if (toolX < workLeft) toolX = workLeft;
    if (toolX + toolW > workRight) toolX = workRight - toolW;

    tool = std::make_unique<ToolVideo>(toolX, toolY, toolW, toolH, this);
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

    SetWindowPos(hwnd,nullptr,0,0,0,0,SWP_NOMOVE |SWP_NOSIZE |SWP_NOZORDER |SWP_NOACTIVATE |SWP_FRAMECHANGED);
}