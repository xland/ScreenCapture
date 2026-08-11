#include "pch.h"
#include <include/Ling.h>
#include "WinVideo.h"
// WinVideoMp4.hpp / WinVideoGif.hpp 里用的是裸 ComPtr，本项目的 pch 没有这条 using，
// 在包含它们之前补上，头文件本身保持原样。
using namespace Microsoft::WRL;
#include "WinVideoMp4.hpp"
#include "WinVideoGif.hpp"
#include "CutMask.h"
#include "../Tool/ToolVideo.h"
#include "../App.h"
#include "../Setting.h"

namespace {
    std::unique_ptr<WinVideo> winVideo;
}

WinVideo::WinVideo() : Ling::WinBase()
{
    setTitle(L"Screen Capture Video");
    auto [x1, y1, w1, h1] = App::get()->getScreenArea();
    this->x = x1; this->y = y1; this->w = (float)w1; this->h = (float)h1;
    onMouseDown.add([this](POINT pos, bool isRight) { this->onDown(pos, isRight); });
    onMouseMove.add([this](POINT pos) { this->onMove(pos); });
    onMouseUp.add([this](POINT pos, bool isRight) { this->onUp(pos, isRight); });
    onKeyDown.add([this](UINT key) { this->onKey(key); });
    onDestroy.add([this]() { this->onClosed(); });
}

WinVideo::~WinVideo()
{
}

void WinVideo::init()
{
    auto ptr = new WinVideo();
    winVideo.reset(ptr);
    ptr->cutMask = std::make_unique<CutMask>(ptr);
    ptr->createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

void WinVideo::stopIfRecording()
{
    if (!winVideo) return;
    if (!winVideo->mp4Param && !winVideo->gifParam) return;
    // 正在录制：先停止编码线程，避免退出时线程与设备卡死
    winVideo->stop();
}

// close() 里 DestroyWindow 之后同步触发 onDestroy，而这条路径通常是从 ToolVideo 的按钮
// 回调进来的（tool 是 WinVideo 的成员）。在这里直接 winVideo.reset() 就是 use-after-free，
// 所以窗口句柄立即销毁，C++ 对象的释放推迟到下一轮消息循环。
void WinVideo::onClosed()
{
    if (isClosed) return;
    isClosed = true;
    stop();
    if (tool) tool->close();
    Ling::App::get()->dq.TryEnqueue([]() {
        winVideo.reset();
        // 用完即走模式下录屏结束就退出进程，与 App 构造里的判断对称
        if (Ling::App::get()->args[L"auto-quit"] == L"true") {
            Ling::App::get()->quit(0);
        }
    });
}

void WinVideo::onCreated()
{
    canvas = body->makeChild<Ling::Canvas>();
    canvas->setSizePercent(100.f, 100.f);
    show();
}

void WinVideo::layout()
{
    Ling::WinBase::layout();
    if (!canvas || !cutMask) return;
    auto ctx = canvas->startPaint();
    if (!ctx) return;
    ctx->Clear(0);
    cutMask->paint(ctx);
    canvas->finishPaint();
}

BOOL WinVideo::setCursor()
{
    SetCursor(LoadCursor(nullptr, isFinishCutMask ? IDC_ARROW : IDC_CROSS));
    return TRUE;
}

void WinVideo::startMp4(bool useSpeaker, bool useMic)
{
    setMouseTransparent(true);
    auto videoTempPath = Setting::get()->getDataPath();
    mp4Param = std::make_unique<WinVideoMp4::DESKTOPCAPTUREPARAMS>();
    mp4Param->VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    // 录制区域先夹回桌面范围，再做对齐 —— 只会往里缩，不会越出桌面。
    // HEVC 编码器要求宽高是偶数，链路中间的 RGB32->NV12 转换还会按对齐后的 stride
    // 去读我们交出去的缓冲区，宽度不是 4 的倍数（stride 凑不满 16 字节）时，
    // 有的驱动编码器会读到缓冲区外面，首帧就崩在驱动里。
    // 所以宽度对齐到 4、高度对齐到 2，代价是最多少录右侧 3 像素、底部 1 像素。
    long rcLeft = std::max((long)x, (long)(x + cutMask->maskRect.left));
    long rcTop = std::max((long)y, (long)(y + cutMask->maskRect.top));
    long rcRight = std::min((long)(x + w), (long)(x + cutMask->maskRect.right));
    long rcBottom = std::min((long)(y + h), (long)(y + cutMask->maskRect.bottom));
    // 左上角往里取整，宽高往下取整，两头都不会超出上面夹好的范围
    rcLeft = (rcLeft + 3) & ~3l;
    rcTop = (rcTop + 1) & ~1l;
    // 框选不可能真的小到 4x2，这里只是别让宽高变成 0：rx 全 0 会被当成"录整屏"
    const long rcW = std::max(4l, (rcRight - rcLeft) & ~3l);
    const long rcH = std::max(2l, (rcBottom - rcTop) & ~1l);
    mp4Param->rx = { rcLeft, rcTop, rcLeft + rcW, rcTop + rcH };
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
    auto videoTempPath = Setting::get()->getDataPath();
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
    if (!mp4Param && !gifParam) return L"";
    hide();
    std::wstring filePath;
    if (mp4Param) {
        mp4Param->MustEnd = true;
        filePath = mp4Param->f;
    }
    if (gifParam) {
        gifParam->isFinish = true;
        filePath = gifParam->path;
    }
    if (captureThread.joinable()) {
        captureThread.join();
    }
    // 参数置空即"已停止"，stopIfRecording / onClosed 再进来时不会重复 join
    mp4Param.reset();
    gifParam.reset();
    return filePath;
}

LRESULT WinVideo::onHitTest(const POINT pos)
{
    if (isMouseTransparent || isFinishCutMask || mp4Param || gifParam) {
        return HTTRANSPARENT;
    }
    return HTCLIENT;
}

void WinVideo::onDown(POINT pos, bool isRight)
{
    if (isRight) {
        close();
        return;
    }
    if (isFinishCutMask) return;
    isMouseDown = true;
    cutMask->startMakeRect(pos);
}

void WinVideo::onMove(POINT pos)
{
    if (isFinishCutMask) return;
    // 按下状态是在拖框，松开状态是在窗口间吸附高亮
    if (isMouseDown) {
        cutMask->makeRect(pos);
    }
    else {
        cutMask->highlight(pos);
    }
}

void WinVideo::onUp(POINT pos, bool isRight)
{
    isMouseDown = false;
    if (isFinishCutMask) return;
    // 框选完成后窗口整体让出鼠标，工具条接管所有交互
    isFinishCutMask = true;
    setMouseTransparent(true);
    makeTool();
}

void WinVideo::onKey(UINT key)
{
    if (key == VK_ESCAPE) {
        close();
    }
}

void WinVideo::makeTool()
{
    tool = std::make_unique<ToolVideo>(this);
    // 尺寸在 ToolVideo 构造里算好了，这里只定位
    const int toolW = (int)(tool->w + 0.5f);
    const int toolH = (int)(tool->h + 0.5f);

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
    const bool fitBelow = (maskBottomScr + gap + toolH) <= workBottom;
    const bool fitAbove = (maskTopScr - gap - toolH) >= workTop;

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

    tool->setPosition(toolX, toolY);
    tool->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
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

    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}
