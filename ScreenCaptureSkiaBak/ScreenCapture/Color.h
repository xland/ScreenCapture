#pragma once
#include <stdint.h>
typedef uint32_t Color;
static constexpr inline Color GetColor(unsigned r, unsigned g, unsigned b, unsigned a = 255) {
	return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

static constexpr inline Color ColorWhite = GetColor(255, 255, 255);
static constexpr inline Color ColorBlack = GetColor(0, 0, 0);
static constexpr inline Color ColorTransparent = GetColor(0, 0, 0,0);