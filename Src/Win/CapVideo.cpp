#include "pch.h"
#include <include/Ling.h>
#include "CapVideo.h"
#include "WinCap.h"
#include "CutMask.h"
#include "../Tool/ToolVideo.h"
#include "../App.h"
#include "../Setting.h"
#include "../Lang.h"
// VideoMp4.hpp / VideoGif.hpp 里用的是裸 ComPtr，本项目的 pch 没有这条 using，
// 在包含它们之前补上，头文件本身保持原样。
using namespace Microsoft::WRL;
#include "VideoMp4.hpp"
#include "VideoGif.hpp"

namespace {
    // MP4 录制失败以前是静默 return，用户那边就是"录完什么都没有"，连缓存文件都不生成。
    // 失败原因基本都在对方机器上（混合显卡笔记本把进程放在独显上跑，桌面复制就不成立），
    // 实测让用户在系统图形设置里给本程序选"节能"（把进程放回核显）能解决，
    // 所以不去区分错误码了，统一给这一句最有用的话。
    // 录制界面的窗口是 WS_EX_TOPMOST 的，弹框得跟上，不然会被压在后面看不见
    void showRecordError(const std::wstring& text, const std::wstring& title)
    {
        MessageBoxW(nullptr, text.data(), title.data(), MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
    }

    // 可能管着录制区域那块屏的 (适配器, 输出) 组合
    struct DupTarget
    {
        CComPtr<IDXGIAdapter1> ad;
        UINT output{ 0 };
        RECT mon{}; //这块屏在虚拟桌面里的位置
    };

    // 按 DXGI 枚举顺序收集所有报告了 rect 所在那块屏的 (适配器, 输出)。
    // 原来 dp.ad / dp.nOutput 全是 0，意思是"默认适配器的第 0 个输出"：双显卡笔记本上
    // 默认适配器可能是独显而内屏挂在核显上，独显的 EnumOutputs(0) 直接失败；
    // 多显示器时也永远只录得到枚举出来的第一块屏。
    // 收成一个列表而不是直接挑一个，是因为混合显卡笔记本上独显和核显会同时报告同一块屏，
    // 而 DuplicateOutput 只在真正拥有这块屏的那个适配器上成立，另一个返回
    // DXGI_ERROR_UNSUPPORTED(0x887A0004) —— 光看 desc 分不出来，只能一个个试
    void collectDupTargets(const RECT& rect, std::vector<DupTarget>& targets)
    {
        HMONITOR hMon = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
        if (!hMon) return;
        CComPtr<IDXGIFactory1> factory;
        if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)) || !factory) return;
        for (UINT i = 0;; ++i) {
            CComPtr<IDXGIAdapter1> ad;
            if (FAILED(factory->EnumAdapters1(i, &ad)) || !ad) break;
            for (UINT j = 0;; ++j) {
                CComPtr<IDXGIOutput> output;
                if (FAILED(ad->EnumOutputs(j, &output)) || !output) break;
                DXGI_OUTPUT_DESC desc{};
                if (FAILED(output->GetDesc(&desc)) || desc.Monitor != hMon) continue;
                targets.push_back({ ad, j, desc.DesktopCoordinates });
            }
        }
    }

    // 把桌面坐标的录制区域换算成某块屏的局部坐标 —— DesktopCapture 的裁剪是相对复制出来
    // 那张纹理算的，纹理原点是这块屏的左上角，不是虚拟桌面的原点。
    // 宽高的对齐要求见 startMp4 里的注释，平移之后得重新对一遍
    RECT toLocalRect(const RECT& rect, const RECT& mon)
    {
        long l = std::max(rect.left, mon.left) - mon.left;
        long t = std::max(rect.top, mon.top) - mon.top;
        long r = std::min(rect.right, mon.right) - mon.left;
        long b = std::min(rect.bottom, mon.bottom) - mon.top;
        l = (l + 3) & ~3l;
        t = (t + 1) & ~1l;
        return { l, t, l + std::max(4l, (r - l) & ~3l), t + std::max(2l, (b - t) & ~1l) };
    }
}

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
    layoutTool();
    tool->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
    // 全屏录制时工具条只能压在录制区内部（选区外面上下都放不下），不摘出去就会被录进去
    App::excludeFromCapture(tool->hwnd);
}

void CapVideo::layoutTool()
{
    if (tool) win->layoutTool(tool.get());
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
    // 编码格式不在这里定，交给下面采集线程里那个"HEVC 不行就退 H.264"的循环
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
    // 文案在这里先取出来按值带进线程：langObj 是 WinRT 对象，不往采集线程里带
    auto errText = Lang::get(L"video.recordFailed");
    auto errTitle = Lang::get(L"about.sysTip");
    captureThread = std::jthread([this, errText, errTitle](std::stop_token st) {
        HRESULT hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) {
            showRecordError(errText, errTitle);
            return;
        }
        hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
            showRecordError(errText, errTitle);
            return;
        }
        // 逐个候选试到桌面复制能成立为止。Prepare 挂得很早（缓存文件都还没建），
        // 所以换个候选整个重来最省事，也不用为了预探测多建一次复制对象。
        // targets 得活到 DesktopCapture 返回：dp.ad 是裸指针，生命周期归我们管
        std::vector<DupTarget> targets;
        collectDupTargets(mp4Param->rx, targets);
        const RECT deskRx = mp4Param->rx; //每个候选都从桌面坐标重新换算
        int code = 0;
        // 编码格式也得按可用性退让。HEVC 文件小得多，但它的编码器不是每台机器都有 ——
        // 有的只带解码，有的靠显卡的硬件 MFT 而显卡没有。缺了的时候 AddStream 照样成功
        //（那一步只是记下输出类型），要到 SetInputMediaType 解析编码器时才失败(-10)。
        // 退回 H.264：它从 Win7 起就带软件编码器，没有装不上的道理，代价是同画质下文件更大
        for (auto codec : { MFVideoFormat_HEVC, MFVideoFormat_H264 }) {
            mp4Param->VIDEO_ENCODING_FORMAT = codec;
            if (targets.empty()) {
                mp4Param->rx = deskRx;
                code = VideoMp4::DesktopCapture(*(mp4Param.get())); //一个都没匹配上，走库原来的默认行为
            }
            for (auto& t : targets) {
                if (mp4Param->MustEnd) break; //已经点了停止，别再起新的
                mp4Param->ad = t.ad;
                mp4Param->nOutput = t.output;
                mp4Param->rx = toLocalRect(deskRx, t.mon);
                code = VideoMp4::DesktopCapture(*(mp4Param.get()));
                if (code != -2) break; //成功了，或者失败在跟适配器无关的地方
            }
            if (mp4Param->MustEnd) break;
            // 只有这两个码是"这台机器认不了这个编码器"，换编码器才有意义
            if (code != -5 && code != -10) break;
        }
        if (code != 0) showRecordError(errText, errTitle);
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

bool CapVideo::onSaveKey(bool toClipboard)
{
    return tool ? tool->onSaveKey(toClipboard) : false;
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
    // 刚开录就点了停止，一帧都没采到 —— 这时 Finalize 出来的是个 0 字节的空壳。
    // 别拿它去走存盘流程（那会白弹一次保存框，存出来还是个打不开的文件），
    // 直接删掉、回空路径，让整个流程无声收场
    if (mp4Param && mp4Param->framesWritten == 0) {
        DeleteFile(filePath.data());
        filePath.clear();
    }
    // 参数置空即"已停止"，stopIfRecording / dispose 再进来时不会重复 join
    mp4Param.reset();
    gifParam.reset();
    return filePath;
}
