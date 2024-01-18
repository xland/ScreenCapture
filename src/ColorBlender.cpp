#include "ColorBlender.h"

SkBitmap bitmap;
SkCanvas* canvas;

SkColor4f ColorBlender::Blend(const SkColor& one, const SkColor& two)
{
	if (bitmap.isNull()) {
		bitmap.allocN32Pixels(1, 1);
		canvas = new SkCanvas(bitmap);
	}
	canvas->drawColor(one);
	canvas->drawColor(two);
	void* pixels = bitmap.getPixels();
	auto colorTarget = *(SkColor*)pixels;
	auto currentColor = SkColor4f::FromColor(colorTarget);
	return currentColor;
}

void ColorBlender::Reset()
{
	if (!bitmap.isNull()) {
		delete canvas;
		bitmap.reset();
	}
}
