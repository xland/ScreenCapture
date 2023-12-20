#pragma once
#include <stdint.h>

typedef uint32_t Color;
static constexpr inline Color GetColor(unsigned r, unsigned g, unsigned b, unsigned a = 255)
{
    //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    //// 使用 to_bytes 进行转换
    //std::string utf8Str = converter.to_bytes(word);
    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}