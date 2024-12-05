#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "include/core/SkBitmap.h"

class ColorBlender
{
public:
	static SkColor4f Blend(const SkColor& one, const SkColor& two);
	static void Reset();
private:
};