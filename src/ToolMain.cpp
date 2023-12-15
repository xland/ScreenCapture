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
    auto winMain = WindowMain::get();
    if (!winMain|| winMain->state < State::tool) {        
        return false;
    }
    if (!toolMainRect.contains(x, y)) {
        if (indexHover >= 0) {
            indexHover = -1;
            winMain->Refresh();
        }        
        return false;
    }
    auto index = (x - toolMainRect.left()) / toolBtnWidth;
    if (index != indexHover) {
        indexHover = index;
        winMain->Refresh();
    }    
    return false;
}

bool ToolMain::OnPaint(SkCanvas* canvas)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool) {
        return false;
    }
    auto mask = CutMask::get();
    toolMainRect.setLTRB(mask->CutRect.right() - toolBoxWidth,mask->CutRect.bottom()+ toolBoxSpan, mask->CutRect.right(), mask->CutRect.bottom() + toolBoxSpan+ toolBoxHeight);
    SkPaint paint;
    paint.setColor(SkColorSetARGB(255, 238, 238, 238));
    canvas->drawRect(toolMainRect, paint);

    if (indexHover >= 0) {
        SkRect bgRect;
        bgRect.setXYWH(toolMainRect.left() + indexHover * toolBtnWidth, toolMainRect.top(), toolBtnWidth, toolBoxHeight);
        paint.setColor(SkColorSetARGB(255, 190, 190, 190));
        canvas->drawRect(bgRect, paint);
    }


    paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    auto font = AppFont::Get();
    auto x = toolMainRect.left() + iconLeftMargin;
    auto y = toolMainRect.centerY() + font->fontIconHeight / 2;
    auto btns = { (const char*)u8"\ue8e8",(const char*)u8"\ue6bc", (const char*)u8"\ue603", (const char*)u8"\ue776", (const char*)u8"\ue601", (const char*)u8"\ue69b", (const char*)u8"\ue6ec", (const char*)u8"\ue82e", (const char*)u8"\ue6be"};
    for(auto& var : btns)
    {
        canvas->drawString(var, x, y, *font->fontIcon, paint);
        x += toolBtnWidth;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x -= iconLeftMargin;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    canvas->drawLine(SkPoint::Make(x, y - font->fontIconHeight-2), SkPoint::Make(x, y+2), paint);
    x += iconLeftMargin;
    paint.setStroke(false);
    paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    btns = { (const char*)u8"\ued85",(const char*)u8"\ued8a"};
    for (auto& var : btns)
    {
        canvas->drawString(var, x, y, *font->fontIcon, paint);
        x += toolBtnWidth;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x -= iconLeftMargin;
    paint.setStroke(true);
    canvas->drawLine(SkPoint::Make(x, y - font->fontIconHeight - 2), SkPoint::Make(x, y + 2), paint);
    x += iconLeftMargin;
    paint.setStroke(false);
    paint.setColor(SkColorSetARGB(255, 30, 30, 30));
    
    btns = { (const char*)u8"\ue6a3",(const char*)u8"\ue62f",(const char*)u8"\ue87f",(const char*)u8"\ue6e7" };
    for (auto& var : btns)
    {
        canvas->drawString(var, x, y, *font->fontIcon, paint);
        x += toolBtnWidth;
    }
    return false;
}

//bool ToolMain::OnPaintFinish(SkCanvas *back)
//{
//    auto winMain = WindowMain::get();
//    if (winMain && winMain->state >= State::tool) {
//        auto mask = CutMask::get();
//        toolMainRect.setLTRB(mask->CutRect.right() - toolBoxWidth,mask->CutRect.bottom()+ toolBoxSpan, mask->CutRect.right(), mask->CutRect.bottom() + toolBoxSpan+ toolBoxHeight);
//        SkPaint paint;
//        //SkRect rect1;
//        //rect1.setLTRB(100, 100, 600, 600);
//        //SkRect rect2;
//        //rect2.setLTRB(200, 200, 400, 400);       
//        //base->drawDRRect(rect1, rect2, paint);
//        paint.setColor(SkColorSetARGB(255, 238, 238, 238));
//        back->drawRect(toolMainRect, paint);
//        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
//        auto font = AppFont::Get();
//        auto x = toolMainRect.left() + iconLeftMargin;
//        auto y = toolMainRect.centerY() + font->fontIconHeight / 2;
//        auto btns = { (const char*)u8"\ue8e8",(const char*)u8"\ue6bc", (const char*)u8"\ue603", (const char*)u8"\ue776", (const char*)u8"\ue601", (const char*)u8"\ue69b", (const char*)u8"\ue6ec", (const char*)u8"\ue82e", (const char*)u8"\ue6be"};
//        for(auto& var : btns)
//        {
//            back->drawString(var, x, y, *font->fontIcon, paint);
//            x += toolBtnWidth;
//        }
//        paint.setColor(SkColorSetARGB(255, 180, 180, 180));
//        x -= iconLeftMargin;
//        paint.setStroke(true);
//        paint.setStrokeWidth(1);
//        paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
//        back->drawLine(SkPoint::Make(x, y - font->fontIconHeight-2), SkPoint::Make(x, y+2), paint);
//        x += iconLeftMargin;
//        paint.setStroke(false);
//        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
//        btns = { (const char*)u8"\ued85",(const char*)u8"\ued8a"};
//        for (auto& var : btns)
//        {
//            back->drawString(var, x, y, *font->fontIcon, paint);
//            x += toolBtnWidth;
//        }
//        paint.setColor(SkColorSetARGB(255, 180, 180, 180));
//        x -= iconLeftMargin;
//        paint.setStroke(true);
//        back->drawLine(SkPoint::Make(x, y - font->fontIconHeight - 2), SkPoint::Make(x, y + 2), paint);
//        x += iconLeftMargin;
//        paint.setStroke(false);
//        paint.setColor(SkColorSetARGB(255, 30, 30, 30));
//
//        btns = { (const char*)u8"\ue6a3",(const char*)u8"\ue62f",(const char*)u8"\ue87f",(const char*)u8"\ue6e7" };
//        for (auto& var : btns)
//        {
//            back->drawString(var, x, y, *font->fontIcon, paint);
//            x += toolBtnWidth;
//        }
//    }
//    return false;
//}

ToolMain::ToolMain()
{
}
