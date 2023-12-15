#include "ToolBase.h"
#include "Icon.h"

ToolBase::ToolBase()
{
    auto icon = Icon::ellipse;
}

ToolBase::~ToolBase()
{
}

void ToolBase::drawBgRect(SkCanvas* canvas, SkPaint& paint)
{
    if (indexSelected >= 0)
    {
        SkRect bgRect = SkRect::MakeXYWH(toolRect.left() + indexSelected * btnWidth + 6,
            toolRect.top() + 6,
            btnWidth - 12,
            height - 12);
        paint.setColor(SkColorSetARGB(255, 228, 238, 255));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
    }
    if (indexHover >= 0 && indexHover != indexSelected)
    {
        SkRect bgRect = SkRect::MakeXYWH(toolRect.left() + indexHover * btnWidth + 6,
            toolRect.top() + 6,
            btnWidth - 12,
            height - 12);
        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
    }
}

void ToolBase::drawBtnCheckable(SkCanvas* canvas, SkPaint& paint, std::initializer_list<const char*>& btns)
{
    //for (auto& var : btns)
    //{
    //    if (fontIndex == indexSelected)
    //    {
    //        paint.setColor(SkColorSetARGB(255, 9, 88, 217));
    //    }
    //    canvas->drawString(var, x, y, *font->fontIcon, paint);
    //    if (fontIndex == indexSelected)
    //    {
    //        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    //    }
    //    x += btnWidth;
    //    fontIndex += 1;
    //}
}
