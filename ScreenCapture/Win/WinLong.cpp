#include "pch.h"
#include "App.h"
#include "Util.h"
#include "Lang.h"
#include "WinLong.h"
#include "Tool/ToolLong.h"
#include "WinCutMask.h"
#include "WinPin.h"
#include "WinVideo.h"

std::unique_ptr<WinLong> winLong;
namespace {
    static constexpr UINT scrollMsgId = 18;
    static constexpr UINT scrollEndMsgId = 19;
    static constexpr int comparisonH = 100;  // 匹配比较用的条带高度

    // 将 BGRA 像素条带转为灰度图
    static std::vector<BYTE> toGrayscale(const BYTE* bgra, int width, int height, int stride)
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
    static int findMostSimilarY(const BYTE* gray1, int gray1H, const BYTE* gray2, int gray2H, int width)
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
WinLong::WinLong(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{
}

WinLong::~WinLong()
{

}

void WinLong::init()
{
    WinVideo::release();
    WinLong::release();
    App::initDevice();
    auto [x, y, w, h] = Util::getDesktopInfo();
    winLong = std::make_unique<WinLong>(x, y, w, h);
    winLong->createWindow(WS_EX_TOPMOST);//WS_EX_TOPMOST
    winLong->cutMask = std::make_unique<WinCutMask>(winLong.get());
    winLong->show();
    UpdateWindow(winLong->hwnd);
}


void WinLong::release()
{
    if (winLong.get()) {
        winLong->close();
        winLong.reset();
        if (App::getArg(L"auto-quit") == L"true") {
            App::exit(0);
        }
        else {
            App::disposeDeviceIfIdle();
        }
    }
}

void WinLong::onCreated()
{
    startCircleR *= dpi;
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.68f), bgBrush.GetAddressOf());
    auto size{ startCircleR * 2 };
    layoutTextStart = App::makeTextLayout(Lang::get(L"long.start"), size, size, 16 * dpi);
}

void WinLong::onDestroy()
{
	if (tool.get()) {
		tool->close();
	}
}

void WinLong::onPaint()
{
    render->Clear(0);
    cutMask->paint();
    paintImgPreview();
    if (isFinish) {
        auto borderRadius{ 4.f * dpi };
        render->FillRoundedRectangle(D2D1::RoundedRect(stopTextRect, borderRadius, borderRadius), bgBrush.Get());
        render->DrawTextLayout({ stopTextRect.left, stopTextRect.top}, layoutTextEnd.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    else if(isShowStartBtn) {
        render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(circleCenter.x, circleCenter.y), startCircleR, startCircleR), bgBrush.Get());
        render->DrawTextLayout({ circleCenter.x - startCircleR, circleCenter.y - startCircleR }, layoutTextStart.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void WinLong::onMouseMove(const int& x, const int& y) {
    if (isFinish) {
        if (isShowStartBtn) {
            isShowStartBtn = false;
            refresh();
        }
    }
    if (isFinishCutMask) {
        circleCenter.x = x;
        circleCenter.y = y;
        if (Util::isInRect(cutMask->maskRect,x,y)) {
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
        cutMask->highlight(x, y);
    }
}
void WinLong::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (isFinishCutMask)  return;
    cutMask->makeRect(x, y);
}
void WinLong::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        WinLong::release();
        return;
    }
    if (isFinishCutMask)  return;
    cutMask->startMakeRect(x, y);
}
void WinLong::onMouseUp(const int& x, const int& y)
{
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

void WinLong::onTimer(const UINT& timerId)
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

void WinLong::onKeyDown(const UINT& key)
{
    if (key == VK_ESCAPE) {
        WinLong::release();
    }
}

BOOL WinLong::onCursor()
{
    if (isFinish) {
        setCursor(IDC_ARROW);
        return TRUE;
    }
    if (isFinishCutMask) {
        if (isShowStartBtn) {
            SetCursor(NULL);          
        }
        else {
            setCursor(IDC_ARROW);          
        }
    }
    else {
        setCursor(IDC_CROSS);
    }
    return TRUE;
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
        std::vector<BYTE> scaledData(previewW * 4 * previewH);
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
        render->CreateBitmap(D2D1::SizeU(previewW, previewH), scaledData.data(), previewW * 4, props, imgPreview.GetAddressOf());
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
    std::vector<BYTE> newResult(rowPix * newResultH);
    // 拷贝旧结果
    CopyMemory(newResult.data(), imgData.data(), imgData.size());
    // 拷贝新截图从 changeStartY 到底部的内容
    for (int row = 0; row < imgH - changeStartY; row++) {
        CopyMemory( newResult.data() + (paintStart + row) * rowPix, data.data() + (changeStartY + row) * rowPix, rowPix);
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
    HRGN rgn1 = CreateRectRgn(0, 0, w, h);
    auto& r = cutMask->maskRect;
    HRGN rgn2 = CreateRectRgn(r.left, r.top, r.right, r.bottom);
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
    if (w - cutMask->maskRect.right - 2*dpi < toolW) {
        pos.x = cutMask->maskRect.left - toolW - cutMask->strokeWidth - 2*dpi;
    }
    else {
        pos.x = cutMask->maskRect.right + cutMask->strokeWidth + 2 * dpi;
    }
    pos.y = cutMask->maskRect.bottom - btnSize;
    ClientToScreen(hwnd, &pos);
    tool = std::make_unique<ToolLong>(pos.x, pos.y, toolW, btnSize, this);
    tool->createWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    tool->show();
}

void WinLong::paintImgPreview()
{
    if (!imgPreview.Get() || !tool) return;
    auto bitmapSize = imgPreview->GetPixelSize();
    float drawW = (float)bitmapSize.width;
    float drawH = (float)bitmapSize.height;
    POINT pos{ tool->x, tool->y - (int)drawH - 2 * dpi };
    ScreenToClient(hwnd, &pos);
    D2D1_RECT_F destRect = D2D1::RectF(pos.x, pos.y, pos.x + drawW, pos.y + drawH);
    render->DrawBitmap(imgPreview.Get(), destRect);
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
        layoutTextEnd = App::makeTextLayout(Lang::get(L"long.tooLong"), FLT_MAX, FLT_MAX, 13 * dpi);
    }
    else {
        layoutTextEnd = App::makeTextLayout(Lang::get(L"long.reachedBottom"), FLT_MAX, FLT_MAX, 13 * dpi);
    }    
    DWRITE_TEXT_METRICS tm = {};
    layoutTextEnd->GetMetrics(&tm);
    auto& maskRect = cutMask->maskRect;
    auto halfX = maskRect.left + (maskRect.right - maskRect.left) / 2;
    auto halfW = tm.width / 2;
    float padding{ 8 * dpi };
    stopTextRect.left = halfX - halfW - padding;
    stopTextRect.top = maskRect.bottom - 30 * dpi - padding;
    stopTextRect.right = halfX + halfW + padding;
    stopTextRect.bottom = cutMask->maskRect.bottom - padding;
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
