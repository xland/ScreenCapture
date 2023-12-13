#pragma once
#include <stdint.h>

typedef uint32_t Color;
static constexpr inline Color GetColor(unsigned r, unsigned g, unsigned b, unsigned a = 255)
{
    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}