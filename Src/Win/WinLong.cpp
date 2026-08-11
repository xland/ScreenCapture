#include "pch.h"
#include <include/Ling.h>
#include "WinLong.h"
#include "CutMask.h"
#include "WinPin.h"
#include "../Tool/ToolLong.h"
#include "../App.h"
#include "../Util.h"
#include "../Lang.h"
using namespace Microsoft::WRL;

namespace {
    std::unique_ptr<WinLong> winLong;
    constexpr UINT scrollMsgId = 18;
    constexpr UINT scrollEndMsgId = 19;
    constexpr int comparisonH = 100;  // 匹配比较用的条带高度

    // 将 BGRA 像素条带转为灰度图
    std::vector<BYTE> toGrayscale(const BYTE* bgra, int width, int height, int stride)
    {
        std::vector<BYTE> gray(width * height);
        for (int y = 0; y < height; y++) {
            const BYTE* src = bgra + y * stride;
            BYTE* dst = gray.data() + y * width;
            for (int x = 0; x < width; x++) {
                dst[x] = (BYTE)((src[x * 4] * 114 + src[x * 4 + 1] * 587 + src[x * 4 + 2] * 299) / 1000);
            }
        }
        return gray;
    }

    // 在 gray1 中搜索与 gray2 最相似的偏移 y（SSD 匹配）
    int findMostSimilarY(const BYTE* gray1, int gray1H, const BYTE* gray2, int gray2H, int width)
    {
        int searchH = gray1H - gray2H + 1;
        if (searchH <= 0) return 0;
        double minError = DBL_MAX;
        int bestY = 0;
        for (int y = 0; y < searchH; y++) {
            double error = 0.0;
            for (int row = 0; row < gray2H && error < minError; row++) {
                const BYTE* row1 = gray1 + (y + row) * width;
                const BYTE* row2 = gray2 + row * width;
                for (int x = 0; x < width; x++) {
                    int diff = (int)row1[x] - (int)row2[x];
                    error += diff * diff;
                }
            }
            if (error < minError) {
                minError = error;
                bestY = y;
            }
        }
        return bestY;
    }
}

WinLong::WinLong() : Ling::WinBase()
{
    setTitle(L"Screen Capture Long");
    auto [x1, y1, w1, h1] = App::get()->getScreenArea();
    this->x = x1; this->y = y1; this->w = (float)w1; this->h = (float)h1;
    onMouseDown.add([this](POINT pos, bool isRight) { this->onDown(pos, isRight); });
    onMouseMove.add([this](POINT pos) { this->onMove(pos); });
    onMouseUp.add([this](POINT pos, bool isRight) { this->onUp(pos, isRight); });
    onTimer.add([this](UINT id) { this->onTimerCB(id); });
    onKeyDown.add([this](UINT key) { this->onKey(key); });
    onDestroy.add([this]() { this->onClosed(); });
}

WinLong::~WinLong()
{
}

void WinLong::init()
{
    auto ptr = new WinLong();
    winLong.reset(ptr);
    ptr->cutMask = std::make_unique<CutMask>(ptr);
    ptr->createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

// close() 里 DestroyWindow 之后同步触发 onDestroy，而这条路径很可能是从 ToolLong 的按钮
// 回调进来的（tool 是 WinLong 的成员）。在这里直接 winLong.reset() 就是 use-after-free，
// 所以窗口句柄立即销毁，C++ 对象的释放推迟到下一轮消息循环。
void WinLong::onClosed()
{
    if (isClosed) return;
    isClosed = true;
    killTimer(scrollMsgId);
    killTimer(scrollEndMsgId);
    if (tool) tool->close();
    Ling::App::get()->dq.TryEnqueue([]() {
        winLong.reset();
        // 用完即走模式下滚动截图结束就退出进程，与 App 构造里的判断对称
        if (Ling::App::get()->args[L"auto-quit"] == L"true") {
            Ling::App::get()->quit(0);
        }
    });
}

void WinLong::onCreated()
{
    startCircleR *= dpi;
    canvas = body->makeChild<Ling::Canvas>();
    canvas->enableSwapChain();
    canvas->setSizePercent(100.f, 100.f);
    auto d2d = Ling::D2D::get();
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.68f), bgBrush.GetAddressOf());
    auto size{ startCircleR * 2 };
    layoutTextStart = Util::makeTextLayout(Lang::get(L"long.start"), size, size, 16 * dpi);
    show();
    UpdateWindow(hwnd);
}

void WinLong::layout()
{
    Ling::WinBase::layout();
    if (!canvas || !cutMask) return;
    auto ctx = canvas->startPaint();
    if (!ctx) return;
    ctx->Clear(0);
    cutMask->paint(ctx);
    paintImgPreview(ctx);
    if (isFinish) {
        auto borderRadius{ 4.f * dpi };
        ctx->FillRoundedRectangle(D2D1::RoundedRect(stopTextRect, borderRadius, borderRadius), bgBrush.Get());
        ctx->DrawTextLayout({ stopTextRect.left, stopTextRect.top }, layoutTextEnd.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    else if (isShowStartBtn) {
        ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F((float)circleCenter.x, (float)circleCenter.y), startCircleR, startCircleR), bgBrush.Get());
        ctx->DrawTextLayout({ circleCenter.x - startCircleR, circleCenter.y - startCircleR }, layoutTextStart.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    canvas->finishPaint();
}

BOOL WinLong::setCursor()
{
    if (isFinish) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        return TRUE;
    }
    if (isFinishCutMask) {
        if (isShowStartBtn) {
            // 开始按钮跟着光标走，藏掉系统光标免得两个东西叠在一起
            SetCursor(NULL);
        }
        else {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
    }
    else {
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
    }
    return TRUE;
}

void WinLong::onDown(POINT pos, bool isRight)
{
    if (isRight) {
        close();
        return;
    }
    if (isFinishCutMask) return;
    isMouseDown = true;
    cutMask->startMakeRect(pos);
}

void WinLong::onMove(POINT pos)
{
    if (isFinish) {
        if (isShowStartBtn) {
            isShowStartBtn = false;
            refresh();
        }
        return;
    }
    if (isFinishCutMask) {
        circleCenter = pos;
        auto& r = cutMask->maskRect;
        if (pos.x > r.left && pos.x < r.right && pos.y > r.top && pos.y < r.bottom) {
            isShowStartBtn = true;
            refresh();
        }
        else {
            if (isShowStartBtn) {
                isShowStartBtn = false;
                refresh();
            }
        }
    }
    else {
        if (isShowStartBtn) {
            isShowStartBtn = false;
            refresh();
        }
        // 按下状态是在拖框，松开状态是在窗口间吸附高亮
        if (isMouseDown) {
            cutMask->makeRect(pos);
        }
        else {
            cutMask->highlight(pos);
        }
    }
}

void WinLong::onUp(POINT pos, bool isRight)
{
    isMouseDown = false;
    if (!isFinishCutMask) {
        isFinishCutMask = true;
        return;
    }
    if (isShowStartBtn) { //按下开始按钮
        isScrolling = true;
        hollowWin();
        makeTool();
        firstStep(); //首次截图
    }
}

void WinLong::onTimerCB(UINT timerId)
{
    if (timerId == scrollMsgId) {
        POINT pt;
        GetCursorPos(&pt);
        auto tarHwnd = WindowFromPoint(pt);
        if (targetHwnd == nullptr) {
            targetHwnd = tarHwnd;
        }
        if (tarHwnd != targetHwnd) return; //鼠标没在截屏区域直接退出，定时器仍在检查
        killTimer(scrollMsgId);
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = -WHEEL_DELTA;
        SendInput(1, &input, sizeof(INPUT));
        setTimer(88, scrollEndMsgId); //滚动开始
    }
    else if (scrollEndMsgId == timerId) {
        killTimer(scrollEndMsgId); //滚动完成
        capStep();
    }
}

void WinLong::onKey(UINT key)
{
    if (key == VK_ESCAPE) {
        close();
    }
}

void WinLong::firstStep()
{
    auto& maskRect = cutMask->maskRect;
    imgW = int(maskRect.right - maskRect.left);
    imgH = int(maskRect.bottom - maskRect.top);
    resultH = imgH;
    capStartPos.x = (int)maskRect.left;
    capStartPos.y = (int)maskRect.top;
    ClientToScreen(hwnd, &capStartPos);
    imgData = Util::captureScreen(capStartPos.x, capStartPos.y, imgW, imgH);
    img1 = imgData;
    makeImgPreview();
    refresh();
    setTimer(88, scrollMsgId); //准备滚动
}

void WinLong::makeImgPreview()
{
    imgPreview.Reset();
    float previewScaleW = tool ? (float)tool->w / (float)imgW : 1.0f;
    int previewW = (int)((float)imgW * previewScaleW);
    int previewH = (int)((float)resultH * previewScaleW);
    if (previewW > 0 && previewH > 0) {
        std::vector<BYTE> scaledData((size_t)previewW * 4 * previewH);
        for (int y = 0; y < previewH; y++) {
            int srcY = (int)((float)y / previewScaleW);
            if (srcY >= resultH) srcY = resultH - 1;
            for (int x = 0; x < previewW; x++) {
                int srcX = (int)((float)x / previewScaleW);
                if (srcX >= imgW) srcX = imgW - 1;
                int srcIdx = (srcY * imgW + srcX) * 4;
                int dstIdx = (y * previewW + x) * 4;
                scaledData[dstIdx] = imgData[srcIdx];
                scaledData[dstIdx + 1] = imgData[srcIdx + 1];
                scaledData[dstIdx + 2] = imgData[srcIdx + 2];
                scaledData[dstIdx + 3] = imgData[srcIdx + 3];
            }
        }
        D2D1_BITMAP_PROPERTIES1 props = {
            .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
            .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_NONE}
        };
        Ling::D2D::get()->deviceContext->CreateBitmap(D2D1::SizeU(previewW, previewH), scaledData.data(), previewW * 4, props, imgPreview.GetAddressOf());
    }
}

void WinLong::capStep()
{
    auto data = Util::captureScreen(capStartPos.x, capStartPos.y, imgW, imgH);
    // 检测滚动区域：首次时找出前后两帧的像素差异边界
    if (firstCheck) {
        changeStartY = -1;
        for (int y = 0; y < imgH; y++) {
            for (int x = 0; x < imgW; x++) {
                int idx = (y * imgW + x) * 4;
                if (img1[idx] != data[idx] || img1[idx + 1] != data[idx + 1] || img1[idx + 2] != data[idx + 2]) {
                    if (changeStartY == -1) changeStartY = y;
                    break;
                }
            }
            if (changeStartY != -1) break;
        }
        if (changeStartY == -1) {
            // 没有检测到变化，可能滚动未生效
            dismissTime++;
            if (dismissTime > 5) { stopCap(); return; }
            setTimer(500, scrollMsgId);
            return;
        }
        firstCheck = false;
    }
    int rowPix{ imgW * 4 };
    // 从 changeStartY 开始，裁剪用于匹配的条带
    int stripH = std::min(comparisonH, imgH - changeStartY);
    if (stripH <= 0) { setTimer(500, scrollMsgId); return; }
    int img1StripH = imgH - changeStartY;
    auto gray1 = toGrayscale(img1.data() + changeStartY * rowPix, imgW, img1StripH, rowPix);
    auto gray2 = toGrayscale(data.data() + changeStartY * rowPix, imgW, stripH, rowPix);
    int y = findMostSimilarY(gray1.data(), img1StripH, gray2.data(), stripH, imgW);
    if (y == 0) { // 未检测到滚动
        dismissTime++;
        if (dismissTime > 2) { stopCap(); return; }
        setTimer(500, scrollMsgId);
        return;
    }
    dismissTime = 0;
    // 计算拼接位置
    int paintStart = resultH - (imgH - y - changeStartY);
    int newResultH = paintStart + (imgH - changeStartY);
    // 创建新的结果图像
    std::vector<BYTE> newResult((size_t)rowPix * newResultH);
    // 拷贝旧结果
    CopyMemory(newResult.data(), imgData.data(), imgData.size());
    // 拷贝新截图从 changeStartY 到底部的内容
    for (int row = 0; row < imgH - changeStartY; row++) {
        CopyMemory(newResult.data() + (size_t)(paintStart + row) * rowPix, data.data() + (size_t)(changeStartY + row) * rowPix, rowPix);
    }
    imgData = std::move(newResult);
    img1 = data;
    resultH = newResultH;
    if (resultH > 20000) { stopCap(); return; }
    makeImgPreview();
    refresh();
    setTimer(500, scrollMsgId); //准备下次滚动
}

void WinLong::hollowWin()
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

void WinLong::makeTool()
{
    auto btnSize{ 32.f * dpi };
    auto toolW{ btnSize * 4 };
    POINT pos{ 0,0 };
    if (w - cutMask->maskRect.right - 2 * dpi < toolW) {
        pos.x = (LONG)(cutMask->maskRect.left - toolW - cutMask->strokeWidth - 2 * dpi);
    }
    else {
        pos.x = (LONG)(cutMask->maskRect.right + cutMask->strokeWidth + 2 * dpi);
    }
    pos.y = (LONG)(cutMask->maskRect.bottom - btnSize);
    ClientToScreen(hwnd, &pos);
    tool = std::make_unique<ToolLong>(this);
    // 尺寸在 ToolLong 构造里算好了，这里只定位；两者都要在建窗口之前设好
    tool->setPosition(pos.x, pos.y);
    tool->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
}

void WinLong::paintImgPreview(ID2D1DeviceContext* ctx)
{
    if (!imgPreview || !tool) return;
    auto bitmapSize = imgPreview->GetPixelSize();
    float drawW = (float)bitmapSize.width;
    float drawH = (float)bitmapSize.height;
    POINT pos{ tool->x, tool->y - (int)drawH - (int)(2 * dpi) };
    ScreenToClient(hwnd, &pos);
    D2D1_RECT_F destRect = D2D1::RectF((float)pos.x, (float)pos.y, pos.x + drawW, pos.y + drawH);
    ctx->DrawBitmap(imgPreview.Get(), destRect);
}

void WinLong::stopCap()
{
    isFinish = true;
    makeStopText();
    SetWindowRgn(hwnd, NULL, TRUE);
    isScrolling = false;
    killTimer(scrollMsgId);
    killTimer(scrollEndMsgId);
    refresh();
}

void WinLong::makeStopText()
{
    if (resultH > 20000) {
        layoutTextEnd = Util::makeTextLayout(Lang::get(L"long.tooLong"), FLT_MAX, FLT_MAX, 13 * dpi);
    }
    else {
        layoutTextEnd = Util::makeTextLayout(Lang::get(L"long.reachedBottom"), FLT_MAX, FLT_MAX, 13 * dpi);
    }
    if (!layoutTextEnd) return;
    DWRITE_TEXT_METRICS tm = {};
    layoutTextEnd->GetMetrics(&tm);
    auto& maskRect = cutMask->maskRect;
    auto halfX = maskRect.left + (maskRect.right - maskRect.left) / 2;
    auto halfW = tm.width / 2;
    float padding{ 8 * dpi };
    stopTextRect.left = halfX - halfW - padding;
    stopTextRect.top = maskRect.bottom - 30 * dpi - padding;
    stopTextRect.right = halfX + halfW + padding;
    stopTextRect.bottom = maskRect.bottom - padding;
    layoutTextEnd->SetMaxWidth(stopTextRect.right - stopTextRect.left);
    layoutTextEnd->SetMaxHeight(stopTextRect.bottom - stopTextRect.top);
}

void WinLong::copyToClipboard()
{
    if (imgData.empty()) return;
    Util::saveToClipboard(imgW, resultH, imgData.data());
}

void WinLong::saveToFile()
{
    if (imgData.empty()) return;
    auto path = Util::getSaveFilePath(hwnd);
    if (path.empty()) return;
    Util::saveToFile(path, imgW, resultH, imgData.data());
}

void WinLong::pin()
{
    if (imgData.empty()) return;
    // 居中放置在主显示器
    auto monitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{ sizeof(MONITORINFO) };
    GetMonitorInfo(monitor, &mi);
    auto& workArea = mi.rcWork;
    int screenW = workArea.right - workArea.left;
    int screenH = workArea.bottom - workArea.top;
    int posX = workArea.left + (screenW - imgW) / 2;
    int posY = workArea.top + (screenH - std::min(resultH, screenH)) / 2;
    WinPin::initFromData(posX, posY, imgW, resultH, imgData);
}
