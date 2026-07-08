#pragma once

#include "pch.h"
#include "gifski.h"
#include "Util.h"
//#include "cgif/cgif.h"
//D:\sdk\gifski\target\release\gifski.lib

namespace WinVideoGif {

    struct GifParam
    {
        bool isFinish;
        std::wstring path;
        int w;
        int h;
        int x;
        int y;
        UINT fps{16};
    };

    inline void drawCursor(HDC hMemDC, GifParam* param) {
        CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
        GetCursorInfo(&cursorInfo);
        if (cursorInfo.flags == CURSOR_SHOWING) {
            ICONINFO iconInfo;
            GetIconInfo(cursorInfo.hCursor, &iconInfo);
            int localX = cursorInfo.ptScreenPos.x - param->x - iconInfo.xHotspot;
            int localY = cursorInfo.ptScreenPos.y - param->y - iconInfo.yHotspot;
            if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
            if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
            if (localX >= 0 && localX < param->w  && localY >= 0 && localY < param->h) {
                DrawIconEx(hMemDC, localX, localY, cursorInfo.hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_DEFAULTSIZE);
            }
        }
    }
    
    //这段代码是好的，但会让exe体积从632增大到1404
    //生成的gif体积很小
    inline void createGif(GifParam* param) {
        GifskiSettings setting{
            .width{(uint32_t)param->w},
            .height{(uint32_t)param->h},
            .quality{80},
            .fast{true},
            .repeat{0}//循环
        };
        auto path = Util::convertToStr(param->path);
        gifski* encoder = gifski_new(&setting);
        gifski_set_file_output(encoder, path.data());
        // 用32位位图采集，天然4字节对齐，无行填充问题
        uint32_t srcRowBytes = param->w * 4;
        std::vector<unsigned char> bgra_buffer(srcRowBytes * param->h);
        // gifski需要紧密排列的RGB数据
        uint32_t dstRowBytes = param->w * 3;
        std::vector<unsigned char> rgb_buffer(dstRowBytes * param->h);
        HDC hScreenDC = GetDC(nullptr);
        HDC hMemDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, param->w, param->h);
        HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);
        BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), param->w, 0-param->h, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
        auto index{ 0 };
        while (!param->isFinish) {
            BitBlt(hMemDC, 0, 0, param->w, param->h, hScreenDC, param->x, param->y, SRCCOPY);
            drawCursor(hMemDC, param);
            GetDIBits(hMemDC, hBitmap, 0, param->h, (void*)bgra_buffer.data(), &bmi, DIB_RGB_COLORS);
            // BGRA → RGB，逐行转换到紧密排列的缓冲区
            for (int row = 0; row < param->h; row++) {
                const unsigned char* srcRow = bgra_buffer.data() + row * srcRowBytes;
                unsigned char* dstRow = rgb_buffer.data() + row * dstRowBytes;
                for (int col = 0; col < param->w; col++) {
                    dstRow[col * 3 + 0] = srcRow[col * 4 + 2]; // R
                    dstRow[col * 3 + 1] = srcRow[col * 4 + 1]; // G
                    dstRow[col * 3 + 2] = srcRow[col * 4 + 0]; // B
                }
            }
            double timestamp_sec = static_cast<double>(index) / param->fps;
            gifski_add_frame_rgb(encoder, index, (uint32_t)param->w, dstRowBytes, (uint32_t)param->h, rgb_buffer.data(), timestamp_sec);
            Sleep(1000 / param->fps);
            index += 1;
        }
        gifski_finish(encoder);
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(nullptr, hScreenDC);
    }

    inline void createGif2(GifParam* param) {
        //auto path = Util::convertToStr(param->path);
        //CGIFrgb_Config config = { 0 };
        //config.path = path.data();
        //config.width = param->w;
        //config.height = param->h;
        //config.attrFlags = 0;
        //config.genFlags = CGIF_FRAME_GEN_USE_DIFF_WINDOW | CGIF_FRAME_GEN_USE_TRANSPARENCY; // 启用帧优化：只编码变化区域 + 透明优化
        //CGIFrgb* pGIF = cgif_rgb_newgif(&config);
        //CGIFrgb_FrameConfig fconfig = { 0 };
        //fconfig.fmtChan = CGIF_CHAN_FMT_RGB;
        //fconfig.delay = static_cast<uint16_t>(std::max(2, static_cast<int>(100.0 / param->fps + 0.5)));
        //fconfig.attrFlags = CGIF_RGB_FRAME_ATTR_NO_DITHERING; // 禁用抖动以减小文件大小
        //const int actualDelayMs = fconfig.delay * 10;
        //uint32_t srcRowBytes = param->w * 4;
        //std::vector<unsigned char> bgra_buffer(srcRowBytes * param->h);
        //uint32_t dstRowBytes = param->w * 3;
        //std::vector<unsigned char> rgb_buffer(dstRowBytes * param->h);
        //HDC hScreenDC = GetDC(nullptr);
        //HDC hMemDC = CreateCompatibleDC(hScreenDC);
        //HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, param->w, param->h);
        //HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);
        //BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), param->w, 0 - param->h, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
        //auto index{ 0 };
        //while (!param->isFinish) {
        //    auto tickStart = GetTickCount64();
        //    BitBlt(hMemDC, 0, 0, param->w, param->h, hScreenDC, param->x, param->y, SRCCOPY);
        //    drawCursor(hMemDC, param);
        //    GetDIBits(hMemDC, hBitmap, 0, param->h, (void*)bgra_buffer.data(), &bmi, DIB_RGB_COLORS);
        //    // BGRA → RGB，逐行转换到紧密排列的缓冲区
        //    for (int row = 0; row < param->h; row++) {
        //        const unsigned char* srcRow = bgra_buffer.data() + row * srcRowBytes;
        //        unsigned char* dstRow = rgb_buffer.data() + row * dstRowBytes;
        //        for (int col = 0; col < param->w; col++) {
        //            dstRow[col * 3 + 0] = srcRow[col * 4 + 2]; // R
        //            dstRow[col * 3 + 1] = srcRow[col * 4 + 1]; // G
        //            dstRow[col * 3 + 2] = srcRow[col * 4 + 0]; // B
        //        }
        //    }
        //    fconfig.pImageData = rgb_buffer.data();
        //    cgif_rgb_addframe(pGIF, &fconfig);
        //    auto elapsed = GetTickCount64() - tickStart;
        //    int sleepTime = actualDelayMs - static_cast<int>(elapsed);
        //    if (sleepTime > 0) {
        //        Sleep(sleepTime);
        //    }
        //    index += 1;
        //}
        //cgif_result r = cgif_rgb_close(pGIF);
        //SelectObject(hMemDC, hOldBitmap);
        //DeleteObject(hBitmap);
        //DeleteDC(hMemDC);
        //ReleaseDC(nullptr, hScreenDC);
    }
}