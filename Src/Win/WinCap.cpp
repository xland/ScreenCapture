#include "pch.h"
#include <include/Ling.h>
#include "WinCap.h"
#include "WinPin.h"
#include "CutMask.h"
#include "../App.h"
#include "../Util.h"
#include "../Lang.h"
#include "CapLong.h"
#include "CapVideo.h"
#include "../Tool/ToolCap.h"
using namespace Microsoft::WRL;

namespace
{
    constexpr float scaleNum{ 5.f }, srcW{ 50.f }, srcH{ 30.f };
    constexpr float pixImgH{ scaleNum * srcH };
    constexpr float pixW{ srcW * scaleNum };
}

std::unique_ptr<WinCap> winCap;

WinCap::WinCap() : Ling::WinBase()
{
	setTitle(L"Screen Capture");
    auto [x1, y1, w1, h1] = App::get()->getScreenArea();
	this->x = x1;this->y = y1;this->w = w1;this->h = h1;
    onMouseDown.add([this](POINT pos, bool isRight) { this->onDown(pos, isRight); });
    onMouseMove.add([this](POINT pos) { this->onMove(pos); });
    onMouseUp.add([this](POINT pos, bool isRight) { this->onUp(pos, isRight); });
    onKeyDown.add([this](UINT key) { this->onKey(key); });
    // 滚动截图的定时器借的是本窗口的，转给 CapLong
    onTimer.add([this](UINT id) { if (capLong) capLong->onTimerCB(id); });
    onDestroy.add([this]() { this->onClosed(); });
    // DPI 变了（用户改了缩放比例）：系统会按新旧缩放比把窗口整体缩放一圈，但本窗口是铺满整个
    // 虚拟桌面的，缩放之后就盖不住桌面了，而且底图、选区（cutMask->maskRect）用的都是物理像素，
    // 窗口一变形它们全部错位，挂在选区上的工具条自然也跟着偏。改缩放不会改分辨率，
    // 桌面还是那么多像素，所以等系统把建议矩形应用完（紧随而来的 WM_SIZE）再把窗口掰回桌面大小。
    // 位置不能在 onDpiChanged 里改 —— 那个事件在建议矩形生效之前触发，改了马上被覆盖
    onDpiChanged.add([this]() { dpiChanged = true; });
    onSizeChanged.add([this]() {
        if (!dpiChanged) return;
        dpiChanged = false;
        auto [x1, y1, w1, h1] = App::get()->getScreenArea();
        this->x = x1; this->y = y1; this->w = w1; this->h = h1;
        SetWindowPos(hwnd, nullptr, x1, y1, (int)w1, (int)h1, SWP_NOZORDER | SWP_NOACTIVATE);
        relayoutTool();
    });
}

WinCap::~WinCap()
{
}

void WinCap::init()
{
    // 双击托盘图标会连着来两下，已经开着就不再建第二个
    if (winCap) return;
    auto ptr = new WinCap();
    winCap.reset(ptr);
	ptr->cutMask = std::make_unique<CutMask>(ptr);
    ptr->createNativeWindow(WS_EX_TOOLWINDOW, WS_POPUP);//WS_EX_TOPMOST
}

WinCap* WinCap::get()
{
    return winCap.get();
}

void WinCap::onCreated()
{
    App::get()->takeScreenShot(x, y, w, h, &screenImg);
	auto d2d = Ling::D2D::get();
    // 画布铺满窗口，走 swap chain（双缓冲）后端，避免调整选区时整帧闪烁
    canvas = body->makeChild<Ling::Canvas>();
    canvas->enableSwapChain();
    canvas->setSizePercent(100.f, 100.f);
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.56f), brushBg.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 1.f, 0.5f), crossBrush.GetAddressOf());
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(hwnd, &pos);
    getPixImg(pos);
    setPixPos(pos);
    show();
}

void WinCap::layout()
{
    Ling::WinBase::layout();
    if (!canvas) return;
    auto ctx = canvas->startPaint();
    if (!ctx) return;
    ctx->Clear(0);
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    if (!hideScreenImg) {
        ctx->DrawBitmap(screenImg.Get(), destRect);
    }
    cutMask->paint(ctx);
    if (capLong) capLong->paint(ctx);
    paintPix(ctx);
    canvas->finishPaint();
}

BOOL WinCap::setCursor()
{
    if (stage == CapStage::Select) {
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
        return TRUE;
    }
    if (stage == CapStage::Long && capLong) {
        capLong->setCursor();
        return TRUE;
    }
    if (stage == CapStage::Adjust) {
        POINT pos{};
        GetCursorPos(&pos);
        ScreenToClient(hwnd, &pos);
        switch (cutMask->hitTest(pos))
        {
        case MaskHit::TopLeft:
        case MaskHit::BottomRight:
            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
            return TRUE;
        case MaskHit::TopRight:
        case MaskHit::BottomLeft:
            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
            return TRUE;
        case MaskHit::Top:
        case MaskHit::Bottom:
            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
            return TRUE;
        case MaskHit::Left:
        case MaskHit::Right:
            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
            return TRUE;
        case MaskHit::Inside:
            SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
            return TRUE;
        default:
            break;
        }
    }
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
    return TRUE;
}

void WinCap::setPixPos(POINT pos)
{
    auto span{ 10 * dpi };
    pixPos.x = int(pos.x + span + dpi);
    if (pixPos.x + pixW > w) {
        pixPos.x = int(pos.x - span - pixW + dpi);
    }
    pixPos.y = int(pos.y + span + dpi);
    auto pixH{ pixImgH + 76.f * dpi };
    if (pixPos.y + pixH > h) {
        pixPos.y = int(pos.y - span - pixH + dpi);
    }
}

void WinCap::getPixImg(POINT pos)
{
    if (isPress || stage != CapStage::Select) return;
    const long sw = static_cast<long>(srcW), sh = static_cast<long>(srcH);
    const long iw = static_cast<long>(w), ih = static_cast<long>(h);
    // 期望的源矩形：以光标为正中心，可以越出屏幕。
    const long wantL = pos.x - sw / 2, wantT = pos.y - sh / 2;
    long vl = std::max(wantL, 0L), vt = std::max(wantT, 0L);
    long vr = std::min(wantL + sw, iw), vb = std::min(wantT + sh, ih);
    // 交集在 pixImg 内的落点：期望矩形左上角为原点，所以减 wantL/wantT（越界时为正偏移）
    const long dl = vl - wantL, dt = vt - wantT;
    if (!pixImg) {
        auto d2d = Ling::D2D::get();
        D2D1_BITMAP_PROPERTIES1 prop{};
        prop.pixelFormat = screenImg->GetPixelFormat();
        prop.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
        screenImg->GetDpi(&prop.dpiX, &prop.dpiY);
        auto hr = d2d->deviceContext->CreateBitmap(D2D1::SizeU(sw, sh), nullptr, 0, &prop, &pixImg);
    }
    auto srcRect = D2D1::RectU(vl, vt, vr, vb);
    auto destPoint = D2D1::Point2U(static_cast<UINT32>(dl), static_cast<UINT32>(dt));
    auto hr = pixImg->CopyFromBitmap(&destPoint, screenImg.Get(), &srcRect);
    // pixImg 里这一帧真正有内容的区域。越界部分留着上一帧的残留，
    // 绘制时靠 DrawBitmap 的 srcRect 把它排除掉，露出取景框自己的底色。
    pixSrcRect = D2D1::RectF((float)dl, (float)dt, float(dl + (vr - vl)), float(dt + (vb - vt)));
}

void WinCap::paintPix(ID2D1DeviceContext* ctx)
{
	if (isPress || stage != CapStage::Select) return;
    D2D1_RECT_F pixRect{ (float)pixPos.x, (float)pixPos.y, pixPos.x + pixW, pixPos.y + pixImgH + 76.f * dpi };
    ctx->FillRectangle(pixRect, brushBg.Get());
    if (pixSrcRect.right > pixSrcRect.left && pixSrcRect.bottom > pixSrcRect.top) {
        D2D1_RECT_F imgRect{
            pixPos.x + pixSrcRect.left * scaleNum,
            pixPos.y + pixSrcRect.top * scaleNum,
            pixPos.x + pixSrcRect.right * scaleNum,
            pixPos.y + pixSrcRect.bottom * scaleNum
        };
        ctx->DrawBitmap(pixImg.Get(), imgRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, &pixSrcRect);
    }
    ctx->DrawRectangle(pixRect, brushBg.Get(),dpi);

    float crossWHalf{ 4.f*dpi };
    auto crossRect0 = D2D1::RectF(pixPos.x, pixPos.y+pixImgH / 2 - crossWHalf, pixPos.x+pixW / 2 - crossWHalf, pixPos.y + pixImgH / 2 + crossWHalf);
    auto crossRect1 = D2D1::RectF(pixPos.x + pixW / 2 + crossWHalf, pixPos.y+pixImgH / 2 - crossWHalf, pixPos.x + pixW, pixPos.y + pixImgH / 2 + crossWHalf);
    auto crossRect2 = D2D1::RectF(pixPos.x + pixW / 2 - crossWHalf, pixPos.y, pixPos.x + pixW / 2 + crossWHalf, pixPos.y + pixImgH / 2 - crossWHalf);
    auto crossRect3 = D2D1::RectF(pixPos.x + pixW / 2 - crossWHalf, pixPos.y+pixImgH / 2 + crossWHalf, pixPos.x + pixW / 2 + crossWHalf, pixPos.y + pixImgH);

    ctx->FillRectangle(crossRect0, crossBrush.Get());
    ctx->FillRectangle(crossRect1, crossBrush.Get());
    ctx->FillRectangle(crossRect2, crossBrush.Get());
    ctx->FillRectangle(crossRect3, crossBrush.Get());

    POINT pos;
	GetCursorPos(&pos);
    HDC hScreen = GetDC(NULL);
    COLORREF cr = GetPixel(hScreen, pos.x, pos.y);
    ReleaseDC(NULL, hScreen);
    auto d2d = Ling::D2D::get();
    float padding{ 7.f * dpi },fontSize{ 10.f*dpi };
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    for (size_t i = 0; i < 4; i++)
    {
        std::wstring str;
        if (i == 0) {
            str = std::format(L"HEX (Ctrl+H) : #{:02X}{:02X}{:02X}", GetRValue(cr), GetGValue(cr), GetBValue(cr));
		}
		else if (i == 1) {
			str = std::format(L"RGB (Ctrl+R) : {},{},{}", GetRValue(cr), GetGValue(cr), GetBValue(cr));
		}
        else if (i == 2) {
            auto [c, m, y1, k] = getCMYK(GetRValue(cr), GetGValue(cr), GetBValue(cr));
            str = std::format(L"CMYK (Ctrl+K) : {},{},{},{}", c, m, y1,k);
        }
        else if (i == 3) {
            str = std::format(L"POS (Ctrl+P) : X:{} Y:{}", pos.x, pos.y);
        }
        d2d->dwriteFactory->CreateTextLayout(str.data(), (UINT32)str.length(), d2d->baseTextFormat.Get(), FLT_MAX, FLT_MAX, &textLayout);
        textLayout->SetFontSize(fontSize, { 0,INT_MAX });
        ctx->DrawTextLayout({ pixPos.x + padding, pixPos.y + pixImgH + padding*(i+1) + fontSize*i }, textLayout.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void WinCap::onKey(UINT key)
{
    auto func = []() {
        POINT pos;
        GetCursorPos(&pos);
        HDC hScreen = GetDC(NULL);
        COLORREF cr = GetPixel(hScreen, pos.x, pos.y);
        ReleaseDC(NULL, hScreen);
        return cr;
    };
    if (key == VK_ESCAPE) {
        close();
    }
    else if (key == 'H' && (GetKeyState(VK_CONTROL) & 0x8000)) {
		auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        wchar_t hex[8];
        swprintf_s(hex, L"#%02X%02X%02X", r, g, b);
        Ling::Util::setTextToClipboard(hex);
        close();
    }
    else if (key == 'R' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        Ling::Util::setTextToClipboard(std::format(L"rgb({},{},{})", r, g, b));
        close();
    }
    else if (key == 'K' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        auto [c, m, y1, k] = getCMYK(r, g, b);
        Ling::Util::setTextToClipboard(std::format(L"cmyk({},{},{},{})", c, m, y1, k));
        close();
    }
    else if (key == 'P' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        POINT pos;
        GetCursorPos(&pos);
        Ling::Util::setTextToClipboard(std::format(L"{},{}", pos.x, pos.y));
        close();
    }
}

std::tuple<int, int, int, int> WinCap::getCMYK(const BYTE& r, const BYTE& g, const BYTE& b)
{
    double R = r / 255.0, G = g / 255.0, B = b / 255.0;
    double K = 1.0 - (std::max)(R, (std::max)(G, B));
    double C = (K == 1.0) ? 0.0 : (1.0 - R - K) / (1.0 - K);
    double M = (K == 1.0) ? 0.0 : (1.0 - G - K) / (1.0 - K);
    double Y = (K == 1.0) ? 0.0 : (1.0 - B - K) / (1.0 - K);
    return std::make_tuple(static_cast<int>(std::round(C * 100)),
        static_cast<int>(std::round(M * 100)),
        static_cast<int>(std::round(Y * 100)),
        static_cast<int>(std::round(K * 100))
    );
}
ComPtr<ID2D1Bitmap1> WinCap::getCutImg()
{
    ComPtr<ID2D1Bitmap1> cutImg;
    auto& maskRect = cutMask->maskRect;
    const UINT32 cw = (UINT32)(maskRect.right - maskRect.left);
    const UINT32 ch = (UINT32)(maskRect.bottom - maskRect.top);
    if (cw == 0 || ch == 0) return cutImg;
    D2D1_BITMAP_PROPERTIES1 prop{};
    prop.pixelFormat = screenImg->GetPixelFormat();
    prop.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
    screenImg->GetDpi(&prop.dpiX, &prop.dpiY);
    Ling::D2D::get()->deviceContext->CreateBitmap(D2D1::SizeU(cw, ch), nullptr, 0, &prop, cutImg.GetAddressOf());
    auto start = D2D1::Point2U(0, 0);
    auto rect = D2D1::RectU((UINT32)maskRect.left, (UINT32)maskRect.top, (UINT32)maskRect.right, (UINT32)maskRect.bottom);
    cutImg->CopyFromBitmap(&start, screenImg.Get(), &rect);
    return cutImg;
}

LRESULT WinCap::onHitTest(const POINT pos)
{
    // 录屏阶段整窗让出鼠标：用户要能直接操作被录的那个应用
    if (isMouseTransparent || stage == CapStage::Video) return HTTRANSPARENT;
    return HTCLIENT;
}

void WinCap::onDown(POINT pos, bool isRight)
{
    if (isRight) {
        close();
        return;
    }
    // 选区框好之后，窗口里任意位置双击都等于点了工具条上的"复制到剪切板"。
    // 双击判定得自己做：Ling 的窗口类没带 CS_DBLCLKS，WM_LBUTTONDBLCLK 根本不会来，
    // 所以拿系统的双击间隔（用户在控制面板里调的那个）和双击判定框来认
    auto now = GetTickCount64();
    bool isDblClick = (now - lastDownTime <= GetDoubleClickTime())
        && std::abs(pos.x - lastDownPos.x) <= GetSystemMetrics(SM_CXDOUBLECLK)
        && std::abs(pos.y - lastDownPos.y) <= GetSystemMetrics(SM_CYDOUBLECLK);
    lastDownTime = now;
    lastDownPos = pos;
    if (isDblClick && stage == CapStage::Adjust) {
        copyToClipboard();
        return;
    }
    if (stage == CapStage::Select) {
        isPress = true;
        cutMask->startMakeRect(pos);
    }
    else if (stage == CapStage::Adjust) {
        // 选区外面按下不是重新框选，而是按落点所在的那一块调对应的边或角
        isPress = true;
        cutMask->startAdjust(pos);
        layoutTool(toolCap.get());
    }
}

void WinCap::onMove(POINT pos)
{
    if (stage == CapStage::Select) {
        if (isPress) {
            cutMask->makeRect(pos);
        }
        else {
            cutMask->highlight(pos);
            getPixImg(pos);
            setPixPos(pos);
            refresh();
        }
    }
    else if (stage == CapStage::Adjust) {
        if (!isPress) return;
        cutMask->adjust(pos);
        // 选区变了，工具条跟着走位
        layoutTool(toolCap.get());
    }
    else if (stage == CapStage::Long && capLong) {
        capLong->onMove(pos);
    }
}

void WinCap::onUp(POINT pos, bool isRight)
{
    if (stage == CapStage::Select) {
        isPress = false;
        // 只是点了一下，又没吸附到任何窗口，那就接着让用户框
        if (!cutMask->hasRect()) return;
        // 按住 Ctrl 框选：跳过调整和工具条，直接钉到桌面上
        if (GetKeyState(VK_CONTROL) & 0x8000) {
            startPin();
            return;
        }
        stage = CapStage::Adjust;
        refresh();  // 收掉放大镜
        makeToolCap();
    }
    else if (stage == CapStage::Adjust) {
        isPress = false;
    }
    else if (stage == CapStage::Long && capLong) {
        capLong->onUp(pos);
    }
}

// close() 里 DestroyWindow 之后同步触发 onDestroy，而这条路径通常是从某个工具条的按钮
// 回调里一路进来的（工具条是 WinCap / CapLong / CapVideo 的成员）。在这里直接
// winCap.reset() 就是 use-after-free，所以窗口句柄立即销毁，C++ 对象的释放推迟到下一轮消息循环。
void WinCap::onClosed()
{
    if (isClosed) return;
    isClosed = true;
    if (capVideo) capVideo->dispose();
    if (capLong) capLong->dispose();
    if (toolCap) toolCap->close();
    Ling::App::get()->dq.TryEnqueue([]() {
        winCap.reset();
        // 用完即走模式下截图结束就退出进程，与 App 构造里的判断对称。
        // 但标注、长截图这两条路是先把图钉到桌面上再关自己的，那种情况下活还没干完，
        // 退出的活交给最后一个关掉的贴图窗口
        if (!WinPin::hasWindow()) {
            if (Ling::App::get()->args[L"--auto-quit"] == L"true") {
                Ling::App::get()->quit(0);
            }
            else {
                App::trimMemoryLater(); //只剩托盘图标了，把显卡那边的缓存还回去
            }
        }
    });
}

void WinCap::stopIfRecording()
{
    if (!winCap || !winCap->capVideo) return;
    // 正在录制：先停止编码线程，避免退出时线程与设备卡死
    winCap->capVideo->stop();
}

void WinCap::makeToolCap()
{
    if (toolCap) {
        layoutTool(toolCap.get());
        toolCap->show();
        return;
    }
    toolCap = std::make_unique<ToolCap>(this);
    // 尺寸在 ToolCap 构造里算好了，这里只定位；两者都要在建窗口之前设好
    layoutTool(toolCap.get());
    toolCap->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
}

void WinCap::relayoutTool()
{
    if (capLong) capLong->layoutTool();
    else if (capVideo) capVideo->layoutTool();
    else if (toolCap) layoutTool(toolCap.get());
}

void WinCap::layoutTool(Ling::WinBase* tool)
{
    if (!tool) return;
    const int toolW = (int)(tool->w + 0.5f);
    const int toolH = (int)(tool->h + 0.5f);
    // maskRect 是本窗口的客户区坐标，换算到屏幕坐标
    const int maskLeftScr = x + (int)cutMask->maskRect.left;
    const int maskTopScr = y + (int)cutMask->maskRect.top;
    const int maskRightScr = x + (int)cutMask->maskRect.right;
    const int maskBottomScr = y + (int)cutMask->maskRect.bottom;

    // 用框选区域所在显示器的工作区判断上/下方是否有足够空间
    RECT maskScrRect{ maskLeftScr, maskTopScr, maskRightScr, maskBottomScr };
    HMONITOR hMon = MonitorFromRect(&maskScrRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(MONITORINFO) };
    GetMonitorInfo(hMon, &mi);

    // 间距按工具条自己所在显示器的缩放算：本窗口铺满整个虚拟桌面，dpi 是系统缩放，
    // 混合缩放的多屏下和选区所在的那块屏不一定是一回事
    const int gap = (int)(cutMask->strokeWidth + 2.f * tool->dpi + 0.5f); // 与框选边框的间距
    const bool fitBelow = (maskBottomScr + gap + toolH) <= mi.rcWork.bottom;
    const bool fitAbove = (maskTopScr - gap - toolH) >= mi.rcWork.top;

    // 工具条右侧与框选区域右侧对齐
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
        const int overlapPad = (int)(3.f * tool->dpi + 0.5f);
        toolX = maskRightScr - toolW - overlapPad;
        toolY = maskBottomScr - toolH - overlapPad;
    }

    // 兜底：不越出所在显示器工作区
    if (toolX < mi.rcWork.left) toolX = mi.rcWork.left;
    if (toolX + toolW > mi.rcWork.right) toolX = mi.rcWork.right - toolW;
    tool->setPosition(toolX, toolY);
}

void WinCap::enterLiveStage()
{
    // 底图是拖框那一刻的静态截图，从这里开始不能再画它 ——
    // 否则录屏和滚动截图从屏幕上拿到的都是这张死图。只留遮罩，选区内是透明的洞。
    hideScreenImg = true;
    if (toolCap) toolCap->hide();
    // 原来的 WinLong / WinVideo 建窗口时就是 topmost，这里补上
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    refresh();
}

void WinCap::startPin()
{
    if (!cutMask->hasRect()) return;
    auto& maskRect = cutMask->maskRect;
    // WinPin 构造里会回头来取 getCutImg()，所以得先把它建起来再关自己
    WinPin::init(int(maskRect.left) + x, int(maskRect.top) + y,
        int(maskRect.right - maskRect.left), int(maskRect.bottom - maskRect.top));
    close();
}

void WinCap::startLong()
{
    if (stage != CapStage::Adjust || !cutMask->hasRect()) return;
    stage = CapStage::Long;
    enterLiveStage();
    // 选区已经定了，接下来光标移进选区会出现"开始"按钮，点一下才真的开始滚
    capLong = std::make_unique<CapLong>(this);
}

void WinCap::startVideo()
{
    if (stage != CapStage::Adjust || !cutMask->hasRect()) return;
    stage = CapStage::Video;
    enterLiveStage();
    capVideo = std::make_unique<CapVideo>(this);
    // ToolCap 原地换成 ToolVideo
    capVideo->makeTool();
}

void WinCap::startMp4(bool useSpeaker, bool useMic)
{
    if (capVideo) capVideo->startMp4(useSpeaker, useMic);
}

void WinCap::startGif()
{
    if (capVideo) capVideo->startGif();
}

std::wstring WinCap::stopRecord()
{
    return capVideo ? capVideo->stop() : L"";
}

void WinCap::layoutLongTool()
{
    if (capLong) capLong->layoutTool();
}

void WinCap::longPin()
{
    if (capLong) capLong->pin();
}

void WinCap::longSaveToFile()
{
    if (capLong) capLong->saveToFile();
}

void WinCap::longCopyToClipboard()
{
    if (capLong) capLong->copyToClipboard();
}

void WinCap::hollowWin()
{
    HRGN rgn1 = CreateRectRgn(0, 0, (int)w, (int)h);
    auto& r = cutMask->maskRect;
    HRGN rgn2 = CreateRectRgn((int)r.left, (int)r.top, (int)r.right, (int)r.bottom);
    CombineRgn(rgn1, rgn1, rgn2, RGN_DIFF);
    DeleteObject(rgn2);
    if (SetWindowRgn(hwnd, rgn1, TRUE) == 0) {
        DeleteObject(rgn1);
    }
}

void WinCap::restoreWin()
{
    SetWindowRgn(hwnd, NULL, TRUE);
}

void WinCap::setMouseTransparent(bool transparent)
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
        isPress = false;
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }
    else {
        exStyle &= ~WS_EX_TRANSPARENT;
        exStyle &= ~WS_EX_LAYERED;
    }
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

// 文字识别由外部插件进程来做，这边只负责把选区里的像素递过去
void WinCap::startOcr()
{
    std::vector<BYTE> pixels;
    int cw{ 0 }, ch{ 0 };
    if (!getCutPixels(pixels, cw, ch)) return;
    // 插件缺失时 openWithImageReader 会打开下载页，同样得让位，所以不看返回值
    Util::openWithImageReader(cw, ch, pixels.data());
    close();
}

void WinCap::startQrcode()
{
    std::vector<BYTE> pixels;
    int cw{ 0 }, ch{ 0 };
    if (!getCutPixels(pixels, cw, ch)) return;
    auto text = Util::decodeQrCode(cw, ch, pixels.data());
    // 先让截图窗口连工具条一起从屏幕上消失，弹框独占桌面。这里只 hide 不 close：
    // close() 是把销毁排进 dq 队列的，而 MessageBox 的模态循环同样在泵消息，
    // 真关了就会在弹框还开着的时候把脚下的 this 抽掉，弹框关闭后再真正退场
    hide();
    if (toolCap) toolCap->hide();
    auto title = Lang::get(L"about.sysTip");
    if (text.empty()) {
        MessageBox(nullptr, Lang::get(L"cap.qrcodeEmpty").data(), title.data(), MB_OK | MB_ICONINFORMATION);
    }
    else {
        auto tip = text + L"\n\n" + Lang::get(L"cap.qrcodeCopy");
        if (MessageBox(nullptr, tip.data(), title.data(), MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
            Ling::Util::setTextToClipboard(text);
        }
    }
    close();
}

void WinCap::saveToFile()
{
    std::vector<BYTE> pixels;
    int cw{ 0 }, ch{ 0 };
    if (!getCutPixels(pixels, cw, ch)) return;
    // 另存为对话框是 WinCap 的附属窗口，而 WinCap 自己不是 topmost，对话框也就待在普通层；
    // ToolCap 却是 topmost 的，topmost 那一层永远盖在普通层之上，于是工具条浮在对话框上面。
    // 所以开对话框前先把工具条降回普通层，关掉之后再压回去
    auto setToolTopmost = [this](bool topmost) {
        if (!toolCap || !toolCap->hwnd) return;
        SetWindowPos(toolCap->hwnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
            0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    };
    setToolTopmost(false);
    auto path = Util::getSaveFilePath(hwnd);
    // 对话框关掉后本窗口会被激活（会盖住降下来的工具条），所以只要还留在截图里，
    // 工具条就得重新压回最上层
    if (path.empty()) { //用户取消了
        setToolTopmost(true);
        return;
    }
    if (Util::saveToFile(path, cw, ch, pixels.data())) {
        close();
    }
    else {
        setToolTopmost(true);
    }
}

void WinCap::copyToClipboard()
{
    std::vector<BYTE> pixels;
    int cw{ 0 }, ch{ 0 };
    if (!getCutPixels(pixels, cw, ch)) return;
    Util::saveToClipboard(cw, ch, pixels.data());
    close();
}

// 从底图上把选区那块像素读回来。screenImg 在 GPU 上，不能直接 Map，
// 得先拷到一块带 CPU_READ 的位图上。
bool WinCap::getCutPixels(std::vector<BYTE>& pixels, int& cw, int& ch)
{
    if (!screenImg || !cutMask->hasRect()) return false;
    auto& maskRect = cutMask->maskRect;
    const UINT32 cutW = (UINT32)(maskRect.right - maskRect.left);
    const UINT32 cutH = (UINT32)(maskRect.bottom - maskRect.top);
    if (cutW == 0 || cutH == 0) return false;
    D2D1_BITMAP_PROPERTIES1 prop{
        .pixelFormat{ screenImg->GetPixelFormat() },
        .dpiX{ 96.0f }, .dpiY{ 96.0f },
        .bitmapOptions{ D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW }
    };
    ComPtr<ID2D1Bitmap1> cpuBmp;
    auto hr = Ling::D2D::get()->deviceContext->CreateBitmap(D2D1::SizeU(cutW, cutH), nullptr, 0, &prop, cpuBmp.GetAddressOf());
    if (FAILED(hr)) return false;
    auto start = D2D1::Point2U(0, 0);
    auto rect = D2D1::RectU((UINT32)maskRect.left, (UINT32)maskRect.top, (UINT32)maskRect.left + cutW, (UINT32)maskRect.top + cutH);
    if (FAILED(cpuBmp->CopyFromBitmap(&start, screenImg.Get(), &rect))) return false;
    D2D1_MAPPED_RECT mapped{};
    if (FAILED(cpuBmp->Map(D2D1_MAP_OPTIONS_READ, &mapped))) return false;
    // mapped.pitch 按 GPU 行对齐，可能大于 cutW*4；剪切板和 WIC 都要求紧凑步长，逐行紧缩
    const UINT32 rowBytes = cutW * 4;
    pixels.resize((size_t)rowBytes * cutH);
    for (UINT32 row = 0; row < cutH; row++)
    {
        auto dst = pixels.data() + (size_t)row * rowBytes;
        CopyMemory(dst, mapped.bits + (size_t)row * mapped.pitch, rowBytes);
        // 底图是 GDI 抓来的，alpha 全 0（它自己是 ALPHA_MODE_IGNORE 所以无所谓），
        // 但 PNG 和剪切板会当真，这里统一按不透明补上
        for (UINT32 i = 3; i < rowBytes; i += 4) dst[i] = 255;
    }
    cpuBmp->Unmap();
    cw = (int)cutW;
    ch = (int)cutH;
    return true;
}
