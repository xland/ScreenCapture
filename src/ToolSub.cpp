#include "ToolSub.h"
#include "State.h"
#include "WindowMain.h"
#include "ToolMain.h"
#include "ToolBtn.h"
#include "Icon.h"

ToolSub *toolSub;

ToolSub::ToolSub()
{
    btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形填充"));
    btns.push_back(std::make_shared<ToolBtn>(Icon::dot, L"线条细",false,true,22, SK_ColorTRANSPARENT,true));
    btns.push_back(std::make_shared<ToolBtn>(Icon::dot, L"线条粗", false, true, 52));
    btns.push_back(std::make_shared<ToolBtn>(Icon::dot, L"线条粗+", false, true, 82));
    btns.push_back(std::make_shared<ToolBtn>(Icon::check, L"红", false, true, 22, SkColorSetARGB(255, 207, 19, 34),true));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黄", false, true, 22, SkColorSetARGB(255, 212, 136, 6)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"绿", false, true, 22, SkColorSetARGB(255, 56, 158, 13)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"青", false, true, 22, SkColorSetARGB(255, 19, 194, 194)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"蓝", false, true, 22, SkColorSetARGB(255, 9, 88, 217)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"紫", false, true, 22, SkColorSetARGB(255, 114, 46, 209)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"粉", false, true, 22, SkColorSetARGB(255, 235, 47, 150)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黑", false, true, 22, SkColorSetARGB(255, 0, 0, 0)));
}

ToolSub::~ToolSub()
{
}

void ToolSub::init()
{
    if (!toolSub)
    {
        toolSub = new ToolSub();
    }
}

ToolSub *ToolSub::get()
{
    return toolSub;
}

bool ToolSub::OnMouseDown(int x, int y)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        return false;
    }
    int index = (x - ToolRect.left()) / ToolBtn::width;
    if (index == 0) {
        btns[0]->isSelected = !btns[0]->isSelected;
        winMain->Refresh();
    }
    else if (index > 0 && index<=3) {
        if (btns[index]->isSelected) {
            return false;
        }
        for (size_t i = 1; i <=3; i++)
        {
            if (btns[i]->isSelected)
            {
                btns[i]->isSelected = false;
                break;
            }
        }
        btns[index]->isSelected = true;
        winMain->Refresh();
    } else if (index > 3) {
        if (btns[index]->isSelected) {
            return false;
        }
        for (size_t i = 4; i < btns.size(); i++)
        {
            if (btns[i]->isSelected) 
            {
                btns[i]->isSelected = false;
                btns[i]->icon = Icon::uncheck;
                break;
            }            
        }
        btns[index]->icon = Icon::check;
        btns[index]->isSelected = true;
        winMain->Refresh();
    }
    return false;
}

bool ToolSub::OnPaint(SkCanvas *canvas)
{
    auto winMain = WindowMain::get();
    if (!winMain || winMain->state < State::tool)
    {
        return false;
    }
    auto toolMain = ToolMain::get();
    if (toolMain->IndexSelected < 0)
    {
        return false;
    }
    auto left = toolMain->ToolRect.left();
    auto top = toolMain->ToolRect.bottom() + MarginTop;
    ToolRect.setXYWH(left, top, btns.size() * ToolBtn::width, ToolBtn::height);
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawRoundRect(ToolRect, 3, 3, paint);
    auto x = ToolRect.left();
    auto y = ToolRect.top();
    for (auto& btn : btns)
    {
        btn->Paint(canvas, paint, x, y);
        x += ToolBtn::width;
    }
    return false;
}
