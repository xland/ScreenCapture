#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "WindowMain.h"
#include "AppFont.h"
#include "include/core/SkTextBlob.h"

ToolMain *toolMain;

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
    if (!toolMain)
    {
        toolMain = new ToolMain();
    }
}

ToolMain *ToolMain::get()
{
    return toolMain;
}

bool ToolMain::OnMouseDown(int x, int y)
{
    return false;
}

bool ToolMain::OnMouseUp(int x, int y)
{
    return false;
}

bool ToolMain::OnMouseMove(int x, int y)
{
    return false;
}

bool ToolMain::OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas)
{
    return false;
}

bool ToolMain::OnPaintFinish(SkCanvas *base)
{
    auto winMain = WindowMain::get();
    if (winMain && winMain->state >= State::tool) {
        auto mask = CutMask::get();
        SkRect rect;
        rect.setLTRB(mask->CutRect.right() - toolBoxWidth,mask->CutRect.bottom()+ toolBoxSpan, mask->CutRect.right(), mask->CutRect.bottom() + toolBoxSpan+ toolBoxHeight);
        SkPaint paint;
        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
        base->drawRect(rect, paint);
        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
        auto font = AppFont::Get();
        auto x = rect.left() + iconLeftMargin;
        auto y = rect.centerY() + font->fontIconHeight / 2;
        auto btns = { (const char*)u8"\ue8e8",(const char*)u8"\ue6bc", (const char*)u8"\ue603", (const char*)u8"\ue776", (const char*)u8"\ue601", (const char*)u8"\ue69b", (const char*)u8"\ue6ec", (const char*)u8"\ue82e", (const char*)u8"\ue6be"};
        for(auto& var : btns)
        {
            base->drawString(var, x, y, *font->fontIcon, paint);
            x += toolBtnWidth;
        }
        paint.setColor(SkColorSetARGB(255, 180, 180, 180));
        x = x -  iconLeftMargin/2;
        paint.setStroke(true);
        paint.setStrokeWidth(2);
        paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
        base->drawLine(SkPoint::Make(x, y - font->fontIconHeight-2), SkPoint::Make(x, y+2), paint);
        x += iconLeftMargin;
        paint.setStroke(false);
        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
        btns = { (const char*)u8"\ued85",(const char*)u8"\ued8a"};
        for (auto& var : btns)
        {
            base->drawString(var, x, y, *font->fontIcon, paint);
            x += toolBtnWidth;
        }
        paint.setColor(SkColorSetARGB(255, 180, 180, 180));
        x = x -  iconLeftMargin/2;
        paint.setStroke(true);
        base->drawLine(SkPoint::Make(x, y - font->fontIconHeight - 2), SkPoint::Make(x, y + 2), paint);
        x += iconLeftMargin;
        paint.setStroke(false);
        paint.setColor(SkColorSetARGB(255, 30, 30, 30));

        btns = { (const char*)u8"\ue6a3",(const char*)u8"\ue62f",(const char*)u8"\ue87f",(const char*)u8"\ue6e7" };
        for (auto& var : btns)
        {
            base->drawString(var, x, y, *font->fontIcon, paint);
            x += toolBtnWidth;
        }
    }
    return false;
}

ToolMain::ToolMain()
{
}
