#include "ToolSub.h"
#include "State.h"
#include "App.h"
#include "WindowBase.h"
#include "ToolMain.h"
#include "ToolBtn.h"
#include "Icon.h"
#include "include/core/SkPath.h"

ToolSub *toolSub;

ToolSub::ToolSub()
{
    
}
ToolSub::~ToolSub()
{

}

void ToolSub::addStrokeWidthBtns(int index)
{
    auto temp = {
        std::make_shared<ToolBtn>(Icon::dot, L"线条细", false, true, 22, SK_ColorTRANSPARENT, true),
        std::make_shared<ToolBtn>(Icon::dot, L"线条粗", false, true, 52),
        std::make_shared<ToolBtn>(Icon::dot, L"线条粗+", false, true, 82)
    };
    btns.insert(btns.begin() + index, temp);
}

void ToolSub::addColorBtns()
{
    btns.push_back(std::make_shared<ToolBtn>(Icon::check, L"红", false, true, 22, SkColorSetARGB(255, 207, 19, 34), true));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黄", false, true, 22, SkColorSetARGB(255, 212, 136, 6)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"绿", false, true, 22, SkColorSetARGB(255, 56, 158, 13)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"青", false, true, 22, SkColorSetARGB(255, 19, 194, 194)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"蓝", false, true, 22, SkColorSetARGB(255, 9, 88, 217)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"紫", false, true, 22, SkColorSetARGB(255, 114, 46, 209)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"粉", false, true, 22, SkColorSetARGB(255, 235, 47, 150)));
    btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黑", false, true, 22, SkColorSetARGB(255, 0, 0, 0)));
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

bool ToolSub::OnMouseDown(const int& x, const int& y)
{
    auto winMain = App::GetWin();
    if (winMain->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        return false;
    }
    int index = (x - ToolRect.left()) / ToolBtn::width;
    if (btns[index]->icon == Icon::dot) {
        if (btns[index]->isSelected) {
            return true;
        }
        for (auto& btn : btns)
        {
            if (btn->isSelected && btn->icon == Icon::dot)
            {
                btn->isSelected = false;
                break;
            }
        }
        btns[index]->isSelected = true;
        winMain->Refresh();
        return true;
    }
    else if (btns[index]->icon == Icon::uncheck) {
        for (auto& btn : btns)
        {
            if (btn->icon == Icon::check)
            {
                btn->isSelected = false;
                btn->icon = Icon::uncheck;
                break;
            }
        }
        btns[index]->icon = Icon::check;
        btns[index]->isSelected = true;
        winMain->Refresh();
        return true;
    }
    if (index == 0) {
        if (btns[0]->isSelected) {
            btns[0]->isSelected = false;
            if (ToolMain::get()->IndexSelected != 5) { //transparen line
                addStrokeWidthBtns(1);
            }            
        }
        else {
            btns[0]->isSelected = true;
            if (ToolMain::get()->IndexSelected != 5) {
                btns.erase(btns.begin() + 1, btns.begin() + 4);
            }
        }
        winMain->Refresh();
    }
    return true;
}

void ToolSub::InitBtns(int mainToolSelectedIndex)
{
    btns.clear();
    switch (mainToolSelectedIndex)
    {
    case 0: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形填充"));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 1: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::ellipseFill, L"椭圆填充"));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 2: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::arrowFill, L"箭头填充", false, true, 22, SK_ColorTRANSPARENT, true));
        addColorBtns();
        break;
    }
    case 3: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::numberFill, L"标号填充", false, true, 22, SK_ColorTRANSPARENT, true));
        addColorBtns();
        break;
    }
    case 4: {
        addStrokeWidthBtns(0);
        addColorBtns();
        break;
    }
    case 5: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::transparent, L"是否透明", false, true, 22, SK_ColorTRANSPARENT, true));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 6: {
        addColorBtns();
        break;
    }
    case 7: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形马赛克区域"));
        addStrokeWidthBtns(1);
        break;
    }
    case 8: {
        btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形橡皮擦区域"));
        addStrokeWidthBtns(1);
        break;
    }
    default:
        break;
    }
}

bool ToolSub::OnPaint(SkCanvas *canvas)
{
    auto winMain = App::GetWin();
    if (winMain->state < State::tool)
    {
        return false;
    }
    auto toolMain = ToolMain::get();
    if (toolMain->IndexSelected < 0)
    {
        return false;
    }
    auto width = btns.size() * ToolBtn::width;
    auto left = toolMain->ToolRect.left();
    auto mainToolBtnCenterPointX = left + toolMain->IndexSelected * ToolBtn::width + ToolBtn::width / 2;
    if (toolMain->IndexSelected > 5) {
        left = mainToolBtnCenterPointX - width / 2;
    }
    auto top = toolMain->ToolRect.bottom() + MarginTop;
    ToolRect.setXYWH(left, top, width, ToolBtn::height);
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawRoundRect(ToolRect, 3, 3, paint);
    SkPath trianglePath;
    trianglePath.moveTo(mainToolBtnCenterPointX, top - MarginTop/3*2);  // 顶点
    trianglePath.lineTo(mainToolBtnCenterPointX - MarginTop, top);  // 左下角
    trianglePath.lineTo(mainToolBtnCenterPointX + MarginTop, top);  // 右下角
    trianglePath.close();
    canvas->drawPath(trianglePath, paint);
    for (auto& btn : btns)
    {
        btn->Paint(canvas, paint, left, top);
        left += ToolBtn::width;
    }
    return false;
}

bool ToolSub::getFill()
{
    return btns[0]->isSelected;
}

int ToolSub::getStroke()
{
    if (btns[1]->isSelected) {
        return 1;
    }
    else if (btns[2]->isSelected) {
        return 2;
    }
    else {
        return 3;
    }
}

SkColor ToolSub::getColor()
{
    auto it = std::find_if(btns.begin(), btns.end(), [](auto& btn) {
        return btn->icon == Icon::check;
        });
    return it->get()->fontColor;
}
