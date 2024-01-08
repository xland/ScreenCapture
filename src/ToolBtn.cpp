#include "ToolBtn.h"
#include "App.h"
#include "WindowBase.h"
#include <commctrl.h>

static int id{ 0 };

ToolBtn::ToolBtn(const char* icon, std::wstring&& tip, bool isDisable, bool selectable, int fontSize, SkColor fontColor, bool isSelected):
	icon{icon},isHover{false},isSelected{isSelected},isDisable{ isDisable },
    selectable{selectable},fontSize{fontSize},fontColor{fontColor}
{
    tipInfo.cbSize = sizeof(TOOLINFO);
    tipInfo.uFlags = TTF_SUBCLASS;
    tipInfo.hwnd = App::GetWin()->hwnd;
    tipInfo.hinst = GetModuleHandle(NULL);
    tipInfo.uId = id;
    RECT rect{0,0,0,0};
    tipInfo.rect = rect;
    tipInfo.lpszText = (LPWSTR)tip.c_str();
    SendMessage(App::GetWin()->hwndToolTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&tipInfo);
    id += 1;
}

ToolBtn::~ToolBtn()
{
    SendMessage(App::GetWin()->hwndToolTip, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&tipInfo);
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
    setToolTip(x, y);
}

void ToolBtn::setToolTip(const int& x, const int& y)
{
    RECT rect{ .left{x},.top{y},.right{x+(int)ToolBtn::width},.bottom{y+(int)ToolBtn::height}};
    tipInfo.rect = rect;
    SendMessage(App::GetWin()->hwndToolTip, TTM_NEWTOOLRECT, 0, (LPARAM)(LPTOOLINFO)&tipInfo);
}
