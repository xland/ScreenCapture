#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "WindowMain.h"
#include "AppFont.h"
#include "include/core/SkTextBlob.h"
#include "Icon.h"
#include "ToolSub.h"

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
    //btns.push_back(std::make_shared<ToolBtn>(Icon::image, L"图片"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::mosaic, L"马赛克"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::eraser, L"橡皮擦"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::undo, L"上一步",true,false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::redo, L"下一步",true,false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::pin, L"钉住截图区",false,false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::save, L"保存", false, false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::close, L"退出", false, false));
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

bool ToolMain::OnMouseDown(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        return false;
    }
    if (IndexHovered == IndexSelected)
    {
        btns[IndexHovered]->isSelected = false;
        IndexSelected = -1;
        winMain->state = State::tool;
        winMain->Refresh();
    }
    else
    {
        if (btns[IndexHovered]->selectable) {
            btns[IndexHovered]->isSelected = true;
            if (IndexSelected >= 0) {
                btns[IndexSelected]->isSelected = false;
            }
            IndexSelected = IndexHovered;
            ToolSub::get()->InitBtns(IndexSelected);
            winMain->state = (State)(IndexSelected + 3);
            winMain->Refresh();
        }             
    }
    return true;
}


bool ToolMain::OnPaint(SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (winMain->state < State::tool)
    {
        return false;
    }
    auto mask = CutMask::get();
    ToolRect.setLTRB(mask->CutRect.right() - btns.size()*ToolBtn::width, 
        mask->CutRect.bottom() + MarginTop, 
        mask->CutRect.right(), 
        mask->CutRect.bottom() + MarginTop + ToolBtn::height);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    canvas->drawRoundRect(ToolRect, 3, 3, paint);
    auto x = ToolRect.left();
    auto y = ToolRect.top();
    for (auto& btn : btns)
    {
        btn->Paint(canvas, paint, x, y);
        x += ToolBtn::width;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x = ToolRect.left()+ToolBtn::width*9;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom()-12), paint);
    x += ToolBtn::width * 2;
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom() - 12), paint);    
    return false;
}


