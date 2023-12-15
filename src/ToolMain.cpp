#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "WindowMain.h"
#include "AppFont.h"
#include "include/core/SkTextBlob.h"
#include "Icon.h"

ToolMain *toolMain;

ToolMain::ToolMain()
{
    btns.push_back(std::make_shared<ToolBtn>(Icon::rect, L"矩形"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::ellipse, L"圆形"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::arrow, L"箭头"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::number, L"标号"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::pen, L"画笔"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::line, L"直线"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::text, L"文本"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::mosaic, L"马赛克"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::eraser, L"橡皮擦"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::undo, L"上一步"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::redo, L"下一步"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::pin, L"钉住截图区"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::save, L"保存"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::close, L"退出"));
    btns[9]->isDisable = true;
    btns[10]->isDisable = true;
}

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
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    if (!toolRect.contains(x, y))
    {
        return false;
    }
    int index = (x - toolRect.left()) / btnWidth;
    if (index == indexSelected)
    {
        indexSelected = -1;
    }
    else
    {
        indexSelected = index;
    }
    winMain->Refresh();
    return false;
}

bool ToolMain::OnMouseUp(int x, int y)
{
    return false;
}

bool ToolMain::OnMouseMove(int x, int y)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    if (!toolRect.contains(x, y))
    {
        if (indexHover >= 0)
        {
            indexHover = -1;
            winMain->Refresh();
        }
        for (auto& btn : btns)
        {
            if (btn->isHover) {
                btn->isHover = false;
                winMain->Refresh();
                break;
            }
        }
        return false;
    }
    int index = (x - toolRect.left()) / btnWidth;    
    if (!btns[index]->isHover)
    {
        for (auto& btn : btns)
        {
            if (btn->isHover) {
                btn->isHover = false;
                break;
            }
        }
        btns[index]->isHover = true;
        winMain->Refresh();
    }
    return false;
}

bool ToolMain::OnPaint(SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    auto mask = CutMask::get();
    toolRect.setLTRB(mask->CutRect.right() - btns.size()*ToolBtn::width, 
        mask->CutRect.bottom() + marginTop, 
        mask->CutRect.right(), 
        mask->CutRect.bottom() + marginTop + ToolBtn::height);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    canvas->drawRoundRect(toolRect, 3, 3, paint);
    auto x = toolRect.left();
    auto y = toolRect.top(); // release下这个值取不出： font->fontIconHeight / 2
    for (auto& btn : btns)
    {
        btn->Paint(canvas, paint, x, y);
        x += ToolBtn::width;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x = toolRect.left()+ToolBtn::width*9;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, toolRect.bottom()-12), paint);
    x += ToolBtn::width * 2;
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, toolRect.bottom() - 12), paint);    
    return false;
}


