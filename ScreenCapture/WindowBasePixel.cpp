#include "WindowBase.h"
#include <sstream>
#include <format>

std::wstring WindowBase::getPixelRgb()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    auto result = std::to_wstring(pixelData[pixelIndex + 2]) + L"," +
        std::to_wstring(pixelData[pixelIndex + 1]) + L"," +
        std::to_wstring(pixelData[pixelIndex]);
    return result;
}
std::wstring WindowBase::getPixelHex()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    std::wstringstream ss;
    int rgb = ((pixelData[pixelIndex + 2] << 16) | (pixelData[pixelIndex + 1] << 8) | pixelData[pixelIndex]);
    ss << std::hex << rgb;
    std::wstring hexColorStr = ss.str();
    int str_length = hexColorStr.length();
    for (int i = 0; i < 6 - str_length; i++) {
        hexColorStr = L"0" + hexColorStr;
    }
    return L"#" + hexColorStr;
}

void WindowBase::drawPixelInfo()
{
    if (pixelY == -999999 && pixelX == -999999) {
        return;
    }
    BLRectI rectDst;
    rectDst.x = pixelX + 10;
    rectDst.y = pixelY + 10;
    rectDst.w = 200;
    rectDst.h = 100;
    if (rectDst.x + rectDst.w > w) {
        rectDst.x -= (rectDst.w + 20);
    }
    if (rectDst.y + rectDst.w > h) {
        rectDst.y -= (rectDst.w + 20);
    }
    {
        auto srcX = pixelX - 10;
        auto srcY = pixelY - 5;
        auto dstX = rectDst.x;
        auto dstY = rectDst.y;
        static size_t dataSize = w * 4 * h;
        for (size_t y = 0; y < 10; y++)
        {
            for (size_t x = 0; x < 20; x++) {
                static BLRgba32 color;
                if (srcX < 0 || srcY < 0 || srcX >= w || srcY >= h) {
                    color.setB(0);
                    color.setG(0);
                    color.setR(0);
                    color.setA(255);
                }
                else
                {
                    size_t indexSrc = srcY * w * 4 + srcX * 4;
                    color.setB(pixelData[indexSrc]);
                    color.setG(pixelData[indexSrc + 1]);
                    color.setR(pixelData[indexSrc + 2]);
                    color.setA(255);
                }
                PaintCtx->setFillStyle(color);
                PaintCtx->fillRect(dstX, dstY, 10, 10);
                dstX += 10;
                srcX += 1;
            }
            dstX = rectDst.x;
            dstY += 10;
            srcX = pixelX - 10;
            srcY += 1;
        }
    }
    PaintCtx->setStrokeStyle(BLRgba32(0, 0, 0));
    PaintCtx->setStrokeWidth(1);
    PaintCtx->strokeRect(BLRectI(rectDst.x, rectDst.y, rectDst.w, rectDst.h * 2));
    {
        int halfCrossW = 5;
        int halfW = rectDst.w / 2;
        int halfH = rectDst.h / 2;
        PaintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 110));
        PaintCtx->setStrokeWidth(halfCrossW * 2);
        PaintCtx->strokeLine(rectDst.x, rectDst.y + halfH, rectDst.x + halfW - halfCrossW, rectDst.y + halfH);
        PaintCtx->strokeLine(rectDst.x + halfW + halfCrossW, rectDst.y + halfH, rectDst.x + rectDst.w, rectDst.y + halfH);
        PaintCtx->strokeLine(rectDst.x + halfW, rectDst.y, rectDst.x + halfW, rectDst.y + halfH - halfCrossW);
        PaintCtx->strokeLine(rectDst.x + halfW, rectDst.y + halfH + halfCrossW, rectDst.x + halfW, rectDst.y + rectDst.h);
    }
    PaintCtx->setFillStyle(BLRgba32(0, 0, 0, 200));
    PaintCtx->fillRect(BLRectI(rectDst.x, rectDst.y + rectDst.h, rectDst.w, rectDst.h));

    auto font = Font::Get()->fontText;
    font->setSize(16.6);
    PaintCtx->setFillStyle(BLRgba32(255, 255, 255));
    auto textX = rectDst.x + 8;
    auto textY = rectDst.y + rectDst.h + 14 + font->metrics().ascent;
    std::string utf8 = std::format("Position:X:{} Y:{}", pixelX, pixelY);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY), *font, utf8.data());
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    utf8 = std::format("RGB(Ctr+R):{},{},{}", pixelData[pixelIndex + 2], pixelData[pixelIndex + 1], pixelData[pixelIndex]);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY + 28), *font, utf8.data());
    std::stringstream ss;
    ss << std::hex << (pixelData[pixelIndex + 2] << 16 | pixelData[pixelIndex + 1] << 8 | pixelData[pixelIndex]);
    std::string hex = ss.str();
    size_t str_length = hex.length();
    for (size_t i = 0; i < 6 - str_length; i++) {
        hex = "0" + hex;
    }
    utf8 = std::format("HEX(Ctr+H):#{}", hex);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY + 56), *font, utf8.data());
}