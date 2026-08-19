#include "pch.h"
#include <include/Ling.h>
#include "CapLong.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinPin.h"
#include "../Tool/ToolLong.h"
#include "../App.h"
#include "../Util.h"
#include "../Lang.h"
using namespace Microsoft::WRL;

namespace {
    constexpr UINT scrollMsgId = 18;
    constexpr UINT scrollEndMsgId = 19;
    constexpr int comparisonH = 100;  // 匹配比较用的条带高度
    // 连续这么多次滚不动才认为到底了。以前是 2 次，反馈里有明明还能滚就提示触底的：
    // 滚轮发出去之后目标窗口不一定跟着动 —— 惯性滚动还没停、页面在加载、
    // 或者鼠标底下那一层刚好不接收滚轮，多试几次就过去了。
    // 每次重试之间隔 500ms，多等几轮的代价只是到底时晚几秒出提示
    constexpr int maxDismissTime = 8;

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

CapLong::CapLong(WinCap* win) : win(win)
{
    startCircleR *= win->dpi;
    auto d2d = Ling::D2D::get();
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.68f), bgBrush.GetAddressOf());
    auto size{ startCircleR * 2 };
    layoutTextStart = Util::makeTextLayout(Lang::get(L"long.start"), size, size, 16 * win->dpi);
    if (layoutTextStart) {
        DWRITE_TEXT_METRICS tm{};
        layoutTextStart->GetMetrics(&tm);
        startTextSize = { tm.width, tm.height };
    }
}

CapLong::~CapLong()
{
}

void CapLong::dispose()
{
    win->killTimer(scrollMsgId);
    win->killTimer(scrollEndMsgId);
    if (tool) tool->close();
}

void CapLong::paint(ID2D1DeviceContext* ctx)
{
    paintImgPreview(ctx);
    if (isFinish) {
        auto borderRadius{ 4.f * win->dpi };
        ctx->FillRoundedRectangle(D2D1::RoundedRect(stopTextRect, borderRadius, borderRadius), bgBrush.Get());
        ctx->DrawTextLayout(stopTextPos, layoutTextEnd.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    else if (isShowStartBtn) {
        ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F((float)circleCenter.x, (float)circleCenter.y), startCircleR, startCircleR), bgBrush.Get());
        ctx->DrawTextLayout({ circleCenter.x - startTextSize.width / 2, circleCenter.y - startTextSize.height / 2 },
            layoutTextStart.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void CapLong::setCursor()
{
    if (!isFinish && isShowStartBtn) {
        // 开始按钮跟着光标走，藏掉系统光标免得两个东西叠在一起
        SetCursor(NULL);
    }
    else {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
    }
}

void CapLong::onMove(POINT pos)
{
    if (isFinish) {
        if (isShowStartBtn) {
            isShowStartBtn = false;
            win->refresh();
        }
        return;
    }
    circleCenter = pos;
    auto& r = win->cutMask->maskRect;
    if (pos.x > r.left && pos.x < r.right && pos.y > r.top && pos.y < r.bottom) {
        isShowStartBtn = true;
        win->refresh();
    }
    else {
        if (isShowStartBtn) {
            isShowStartBtn = false;
            win->refresh();
        }
    }
}

void CapLong::onUp(POINT pos)
{
    if (isScrolling || isFinish) return;
    if (isShowStartBtn) { //按下开始按钮
        isScrolling = true;
        win->hollowWin();
        makeTool();
        firstStep(); //首次截图
    }
}

void CapLong::onTimerCB(UINT timerId)
{
    if (timerId == scrollMsgId) {
        POINT pt;
        GetCursorPos(&pt);
        auto tarHwnd = WindowFromPoint(pt);
        if (targetHwnd == nullptr) {
            targetHwnd = tarHwnd;
        }
        if (tarHwnd != targetHwnd) return; //鼠标没在截屏区域直接退出，定时器仍在检查
        win->killTimer(scrollMsgId);
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = -WHEEL_DELTA;
        SendInput(1, &input, sizeof(INPUT));
        win->setTimer(88, scrollEndMsgId); //滚动开始
    }
    else if (scrollEndMsgId == timerId) {
        win->killTimer(scrollEndMsgId); //滚动完成
        capStep();
    }
}

void CapLong::firstStep()
{
    auto& maskRect = win->cutMask->maskRect;
    imgW = int(maskRect.right - maskRect.left);
    imgH = int(maskRect.bottom - maskRect.top);
    resultH = imgH;
    capStartPos.x = (int)maskRect.left;
    capStartPos.y = (int)maskRect.top;
    ClientToScreen(win->hwnd, &capStartPos);
    imgData = Util::captureScreen(capStartPos.x, capStartPos.y, imgW, imgH);
    img1 = imgData;
    makeImgPreview();
    win->refresh();
    win->setTimer(88, scrollMsgId); //准备滚动
}

void CapLong::makeImgPreview()
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

void CapLong::capStep()
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
            if (dismissTime > maxDismissTime) { stopCap(); return; }
            win->setTimer(500, scrollMsgId);
            return;
        }
        firstCheck = false;
    }
    int rowPix{ imgW * 4 };
    // 从 changeStartY 开始，裁剪用于匹配的条带
    int stripH = std::min(comparisonH, imgH - changeStartY);
    if (stripH <= 0) { win->setTimer(500, scrollMsgId); return; }
    int img1StripH = imgH - changeStartY;
    auto gray1 = toGrayscale(img1.data() + changeStartY * rowPix, imgW, img1StripH, rowPix);
    auto gray2 = toGrayscale(data.data() + changeStartY * rowPix, imgW, stripH, rowPix);
    int y = findMostSimilarY(gray1.data(), img1StripH, gray2.data(), stripH, imgW);
    if (y == 0) { // 未检测到滚动
        dismissTime++;
        if (dismissTime > maxDismissTime) { stopCap(); return; }
        win->setTimer(500, scrollMsgId);
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
    if (resultH > 36000) { stopCap(); return; }
    makeImgPreview();
    win->refresh();
    win->setTimer(500, scrollMsgId); //准备下次滚动
}

void CapLong::makeTool()
{
    tool = std::make_unique<ToolLong>(win);
    // 尺寸在 ToolLong 构造里算好了，这里只定位；两者都要在建窗口之前设好
    layoutTool();
    tool->createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, WS_POPUP);
}

void CapLong::layoutTool()
{
    if (!tool) return;
    // 宽高一律现问 tool 要，不再按 dpi 自己算：DPI 变化后工具条会重算尺寸再回头调这里
    auto toolW{ tool->w };
    POINT pos{ 0,0 };
    auto& cutMask = win->cutMask;
    if (win->w - cutMask->maskRect.right - 2 * win->dpi < toolW) {
        pos.x = (LONG)(cutMask->maskRect.left - toolW - cutMask->strokeWidth - 2 * win->dpi);
    }
    else {
        pos.x = (LONG)(cutMask->maskRect.right + cutMask->strokeWidth + 2 * win->dpi);
    }
    pos.y = (LONG)(cutMask->maskRect.bottom - tool->h);
    ClientToScreen(win->hwnd, &pos);
    tool->setPosition(pos.x, pos.y);
}

void CapLong::paintImgPreview(ID2D1DeviceContext* ctx)
{
    if (!imgPreview || !tool) return;
    auto bitmapSize = imgPreview->GetPixelSize();
    float drawW = (float)bitmapSize.width;
    float drawH = (float)bitmapSize.height;
    POINT pos{ tool->x, tool->y - (int)drawH - (int)(2 * win->dpi) };
    ScreenToClient(win->hwnd, &pos);
    D2D1_RECT_F destRect = D2D1::RectF((float)pos.x, (float)pos.y, pos.x + drawW, pos.y + drawH);
    ctx->DrawBitmap(imgPreview.Get(), destRect);
}

void CapLong::stopCap()
{
    isFinish = true;
    makeStopText();
    win->restoreWin();
    isScrolling = false;
    win->killTimer(scrollMsgId);
    win->killTimer(scrollEndMsgId);
    win->refresh();
}

void CapLong::makeStopText()
{
    if (resultH > 36000) {
        layoutTextEnd = Util::makeTextLayout(Lang::get(L"long.tooLong"), FLT_MAX, FLT_MAX, 13 * win->dpi);
    }
    else {
        layoutTextEnd = Util::makeTextLayout(Lang::get(L"long.reachedBottom"), FLT_MAX, FLT_MAX, 13 * win->dpi);
    }
    if (!layoutTextEnd) return;
    DWRITE_TEXT_METRICS tm = {};
    layoutTextEnd->GetMetrics(&tm);
    auto& maskRect = win->cutMask->maskRect;
    auto halfX = maskRect.left + (maskRect.right - maskRect.left) / 2;
    auto halfW = tm.width / 2;
    float padding{ 8 * win->dpi };
    stopTextRect.left = halfX - halfW - padding;
    stopTextRect.top = maskRect.bottom - 30 * win->dpi - padding;
    stopTextRect.right = halfX + halfW + padding;
    stopTextRect.bottom = maskRect.bottom - padding;
    layoutTextEnd->SetMaxWidth(stopTextRect.right - stopTextRect.left);
    layoutTextEnd->SetMaxHeight(stopTextRect.bottom - stopTextRect.top);
    // 圆角矩形是按文本宽度加 padding 撑出来的，文本本身要摆回它的正中
    stopTextPos = { halfX - halfW, stopTextRect.top + (stopTextRect.bottom - stopTextRect.top - tm.height) / 2 };
}

void CapLong::copyToClipboard()
{
    if (imgData.empty()) return;
    Util::saveToClipboard(imgW, resultH, imgData.data());
}

bool CapLong::saveToFile()
{
    if (imgData.empty()) return false;
    auto path = Util::getSaveFilePath(win->hwnd);
    if (path.empty()) return false;
    return Util::saveToFile(path, imgW, resultH, imgData.data());
}

void CapLong::pin()
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
