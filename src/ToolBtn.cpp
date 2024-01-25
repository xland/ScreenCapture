#include "ToolBtn.h"
#include "App.h"
#include "WindowBase.h"

static int id{ 0 };

ToolBtn::ToolBtn(const char* icon, std::wstring&& tip, bool isDisable, bool selectable, int fontSize, SkColor fontColor, bool isSelected):
    Icon{icon}, IsHover{false}, IsSelected{isSelected}, IsDisable{ isDisable },
    Selectable{selectable},fontSize{fontSize}, FontColor{fontColor}
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
    if (IsSelected && Selectable)
    {
        SkRect bgRect = SkRect::MakeXYWH(x + 6, y + 6, ToolBtn::Width - 12, ToolBtn::Height - 12);
        paint.setColor(SkColorSetARGB(255, 228, 238, 255));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
        paint.setColor(SkColorSetARGB(255, 9, 88, 217));
    }
    else if (IsHover && !IsDisable)
    {
        SkRect bgRect = SkRect::MakeXYWH(x + 6, y + 6, ToolBtn::Width - 12, ToolBtn::Height - 12);
        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
        canvas->drawRoundRect(bgRect, 6, 6, paint);
        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    }
    else
    {
        if (IsDisable) {
            paint.setColor(SkColorSetARGB(255, 160, 160, 160));
        }
        else
        {
            paint.setColor(SkColorSetARGB(255, 30, 30, 30));
        }        
    }
    auto font = App::GetFontIcon();
    font->setSize(fontSize);
    if (FontColor != SK_ColorTRANSPARENT) {
        paint.setColor(FontColor);
    }
    if (fontSize == 22) {
        canvas->drawString(Icon, x + 14, y + ToolBtn::Height / 2 + 8, *font, paint);
    }
    else if (fontSize == 52) {
        canvas->drawString(Icon, x-1, y + ToolBtn::Height / 2 + 19, *font, paint);
    }
    else if (fontSize == 82) {
        canvas->drawString(Icon, x - 16, y + ToolBtn::Height / 2 + 29.5, *font, paint);
    }
    setToolTip(x, y);
}

void ToolBtn::setToolTip(const int& x, const int& y)
{
    RECT rect{ .left{x},.top{y},.right{x+(int)ToolBtn::Width},.bottom{y+(int)ToolBtn::Height}};
    tipInfo.rect = rect;
    SendMessage(App::GetWin()->hwndToolTip, TTM_NEWTOOLRECT, 0, (LPARAM)(LPTOOLINFO)&tipInfo);
}
