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

void WinVideo::startMp4(bool useSpeaker, bool useMic)
{
    auto videoTempPath = App::getDataPath();
    auto pathStr = videoTempPath.append(Util::createFileName(L"mp4")).wstring();
    dp.VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
    dp.rx = { (long)(x + cutMask->maskRect.left), (long)(y+ cutMask->maskRect.top),
        (long)(cutMask->maskRect.right- cutMask->maskRect.left), (long)(cutMask->maskRect.bottom - cutMask->maskRect.top) };
    dp.f = pathStr.data();
    std::vector<WinVideoMp4::VISTAMIXER> vistamixers;
    WinVideoMp4::EnumVistaMixers(vistamixers);
    dp.EndMS = 0;
    dp.fps = 30;
    dp.vbrm = 2;
    dp.vbrq = 50;
    dp.Qu = 50;
    dp.MustEnd = false;
    WinVideoMp4::setAudio(dp, useSpeaker, useMic);
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

void WinVideo::startGif()
{
    captureThread = std::jthread([this](std::stop_token st) {
        auto fps{ 30 };
        auto videoTempPath = App::getDataPath();
        auto pathStr = videoTempPath.append(Util::createFileName(L"gif")).string();
        int x{ (int)(this->x + cutMask->maskRect.left) }, y{ (int)(this->y + cutMask->maskRect.top) };
        int width{ (int)(cutMask->maskRect.right - cutMask->maskRect.left) };
        int height{ (int)(cutMask->maskRect.bottom - cutMask->maskRect.top) };

        GifskiSettings setting;
        setting.fast = true;
        setting.width = (uint32_t)width;
        setting.height = (uint32_t)height;
        setting.repeat = true;
        setting.quality = 80;

        gifski* encoder = gifski_new(&setting);
        gifski_set_file_output(encoder, pathStr.data());

        uint32_t rowBytes = width * 3;
        std::vector<unsigned char> bgr_buffer(rowBytes * height);
        HDC hScreenDC = GetDC(nullptr);
        HDC hMemDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
        HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);
        BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), width, -height, 1, 24, BI_RGB, (DWORD)width * 4 * height, 0, 0, 0, 0 };

        auto index{ 0 };
        while (!isFinishGifRecord) {
            BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);
            CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
            GetCursorInfo(&cursorInfo);
            if (cursorInfo.flags == CURSOR_SHOWING) {
                int localX = cursorInfo.ptScreenPos.x - x;
                int localY = cursorInfo.ptScreenPos.y - y;
                if (localX >= 0 && localX < width && localY >= 0 && localY < height) {
                    DrawIconEx(hMemDC, localX, localY, cursorInfo.hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_DEFAULTSIZE);
                }
            }

            GetDIBits(hMemDC, hBitmap, 0, height, (void*)bgr_buffer.data(), &bmi, DIB_RGB_COLORS);
            for (size_t i = 0; i < bgr_buffer.size(); i += 3) {
                std::swap(bgr_buffer[i], bgr_buffer[i + 2]);
            }
            double timestamp_sec = static_cast<double>(index) / fps;
            gifski_add_frame_rgb(encoder, index, width, rowBytes, height, bgr_buffer.data(), timestamp_sec);

            Sleep(1000 / fps);
            index += 1;
        }

        gifski_finish(encoder);
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(nullptr, hScreenDC);
    });
}

void WinVideo::stop()
{
    dp.MustEnd = true;
    isFinishGifRecord = true;
    if (captureThread.joinable()) {
        captureThread.join();
    }
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

void WinVideo::drawCursor(HDC hMemDC) {
    //CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
    //GetCursorInfo(&cursorInfo);
    //if (cursorInfo.flags == CURSOR_SHOWING) {
    //    // 转换为相对于截图区域的坐标
    //    int localX = cursorInfo.ptScreenPos.x - x;
    //    int localY = cursorInfo.ptScreenPos.y - y;
    //    // 检查光标是否在截图区域内
    //    if (localX >= 0 && localX < width && localY >= 0 && localY < height) {
    //        // 将图标绘制到内存 DC 上
    //        DrawIconEx(hMemDC, localX, localY, cursorInfo.hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_DEFAULTSIZE);
    //    }
    //}
}