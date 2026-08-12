#include "pch.h"
#include <include/Ling.h>
#include "CapVideo.h"
#include "WinCap.h"
#include "CutMask.h"
#include "../Tool/ToolVideo.h"
#include "../App.h"
#include "../Setting.h"
// VideoMp4.hpp / VideoGif.hpp 里用的是裸 ComPtr，本项目的 pch 没有这条 using，
// 在包含它们之前补上，头文件本身保持原样。
using namespace Microsoft::WRL;
#include "VideoMp4.hpp"
#include "VideoGif.hpp"

CapVideo::CapVideo(WinCap* win) : win(win)
{
    // 区域已经定了，接下来所有交互都归工具条，宿主窗口整体让出鼠标
    win->setMouseTransparent(true);
}

CapVideo::~CapVideo()
{
}

void CapVideo::makeTool()
{
    tool = std::make_unique<ToolVideo>(win);
    // 尺寸在 ToolVideo 构造里算好了，这里只定位；两者都要在建窗口之前设好
    win->layoutTool(tool.get());
    tool->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
}

void CapVideo::dispose()
{
    stop();
    if (tool) tool->close();
}

bool CapVideo::isRecording() const
{
    return mp4Param || gifParam;
}

void CapVideo::startMp4(bool useSpeaker, bool useMic)
{
    auto& cutMask = win->cutMask;
    auto videoTempPath = Setting::get()->getDataPath();
    mp4Param = std::make_unique<VideoMp4::DESKTOPCAPTUREPARAMS>();
    mp4Param->VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    // 录制区域先夹回桌面范围，再做对齐 —— 只会往里缩，不会越出桌面。
    // HEVC 编码器要求宽高是偶数，链路中间的 RGB32->NV12 转换还会按对齐后的 stride
    // 去读我们交出去的缓冲区，宽度不是 4 的倍数（stride 凑不满 16 字节）时，
    // 有的驱动编码器会读到缓冲区外面，首帧就崩在驱动里。
    // 所以宽度对齐到 4、高度对齐到 2，代价是最多少录右侧 3 像素、底部 1 像素。
    long rcLeft = std::max((long)win->x, (long)(win->x + cutMask->maskRect.left));
    long rcTop = std::max((long)win->y, (long)(win->y + cutMask->maskRect.top));
    long rcRight = std::min((long)(win->x + win->w), (long)(win->x + cutMask->maskRect.right));
    long rcBottom = std::min((long)(win->y + win->h), (long)(win->y + cutMask->maskRect.bottom));
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
    VideoMp4::setAudio(mp4Param.get(), useSpeaker, useMic);
    captureThread = std::jthread([this](std::stop_token st) {
        HRESULT hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) return;
        hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) return;
        VideoMp4::DesktopCapture(*(mp4Param.get()));
        CoUninitialize();
        MFShutdown();
    });
}

void CapVideo::startGif()
{
    auto& cutMask = win->cutMask;
    auto videoTempPath = Setting::get()->getDataPath();
    gifParam = std::make_unique<VideoGif::GifParam>();
    gifParam->x = (int)(win->x + cutMask->maskRect.left);
    gifParam->y = (int)(win->y + cutMask->maskRect.top);
    gifParam->w = (int)(cutMask->maskRect.right - cutMask->maskRect.left);
    gifParam->h = (int)(cutMask->maskRect.bottom - cutMask->maskRect.top);
    gifParam->path = videoTempPath.append(L"temp.gif").wstring();
    captureThread = std::jthread([this](std::stop_token st) {
        VideoGif::createGif(gifParam.get());
    });
}

std::wstring CapVideo::stop()
{
    if (!mp4Param && !gifParam) return L"";
    // 遮罩也别留在屏幕上，录完这一帧就该收工了
    win->hide();
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
    // 参数置空即"已停止"，stopIfRecording / dispose 再进来时不会重复 join
    mp4Param.reset();
    gifParam.reset();
    return filePath;
}
