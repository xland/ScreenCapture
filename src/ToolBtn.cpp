#include "ToolBtn.h"
#include "App.h"

ToolBtn::ToolBtn(const char* icon, std::wstring&& tip, bool isDisable, bool selectable, int fontSize, SkColor fontColor, bool isSelected):
	icon{icon},tip{tip},isHover{false},isSelected{isSelected},isDisable{ isDisable },
    selectable{selectable},fontSize{fontSize},fontColor{fontColor}
{
}

ToolBtn::~ToolBtn()
{
}

void ToolBtn::Paint(SkCanvas* canvas, SkPaint& paint, float& x, float& y)
{
    if (isSelected && selectable)
    {
        SkRect bgRect = SkRect::MakeXYWH(x + 6, y + 6, ToolBtn::width - 12, ToolBtn::height - 12);
        paint.setColor(SkColorSetARGB(255, 228, 238, 255));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
        paint.setColor(SkColorSetARGB(255, 9, 88, 217));
    }
    else if (isHover && !isDisable)
    {
        SkRect bgRect = SkRect::MakeXYWH(x + 6, y + 6, ToolBtn::width - 12, ToolBtn::height - 12);
        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    }
    else
    {
        if (isDisable) {
            paint.setColor(SkColorSetARGB(255, 160, 160, 160));
        }
        else
        {
            paint.setColor(SkColorSetARGB(255, 30, 30, 30));
        }        
    }
    auto font = App::GetFontIcon();
    font->setSize(fontSize);
    if (fontColor != SK_ColorTRANSPARENT) {
        paint.setColor(fontColor);
    }
    if (fontSize == 22) {
        canvas->drawString(icon, x + 14, y + ToolBtn::height / 2 + 8, *font, paint);
    }
    else if (fontSize == 52) {
        canvas->drawString(icon, x-1, y + ToolBtn::height / 2 + 19, *font, paint);
    }
    else if (fontSize == 82) {
        canvas->drawString(icon, x - 16, y + ToolBtn::height / 2 + 29.5, *font, paint);
    }
    
}
