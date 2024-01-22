#include "ToolSub.h"
#include "State.h"
#include "App.h"
#include "WindowBase.h"
#include "ToolMain.h"
#include "ToolBtn.h"
#include "Icon.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"

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
    Btns.insert(Btns.begin() + index, temp);
}
void ToolSub::addColorBtns()
{
    Btns.push_back(std::make_shared<ToolBtn>(Icon::check, L"红", false, true, 22, SkColorSetARGB(255, 207, 19, 34), true));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黄", false, true, 22, SkColorSetARGB(255, 212, 136, 6)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"绿", false, true, 22, SkColorSetARGB(255, 56, 158, 13)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"青", false, true, 22, SkColorSetARGB(255, 19, 194, 194)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"蓝", false, true, 22, SkColorSetARGB(255, 9, 88, 217)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"紫", false, true, 22, SkColorSetARGB(255, 114, 46, 209)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"粉", false, true, 22, SkColorSetARGB(255, 235, 47, 150)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::uncheck, L"黑", false, true, 22, SkColorSetARGB(255, 0, 0, 0)));
}
void ToolSub::Init()
{
    toolSub = new ToolSub();
}
ToolSub *ToolSub::Get()
{
    return toolSub;
}
bool ToolSub::OnMouseDown(const int& x, const int& y)
{
    isMouseDown = true;
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        return false;
    }
    int index = (x - ToolRect.left()) / ToolBtn::Width;
    if (Btns[index]->Icon == Icon::dot) {
        if (Btns[index]->IsSelected) {
            return true;
        }
        for (auto& btn : Btns)
        {
            if (btn->IsSelected && btn->Icon == Icon::dot)
            {
                btn->IsSelected = false;
                break;
            }
        }
        Btns[index]->IsSelected = true;
        setRect();
        win->Refresh();
        return true;
    }
    else if (Btns[index]->Icon == Icon::uncheck) {
        for (auto& btn : Btns)
        {
            if (btn->Icon == Icon::check)
            {
                btn->IsSelected = false;
                btn->Icon = Icon::uncheck;
                break;
            }
        }
        Btns[index]->Icon = Icon::check;
        Btns[index]->IsSelected = true;
        setRect();
        win->Refresh();
        return true;
    }
    if (index == 0) {
        if (Btns[0]->IsSelected) {
            Btns[0]->IsSelected = false;
            if (ToolMain::Get()->IndexSelected != 5) { //transparen line
                addStrokeWidthBtns(1);
            }            
        }
        else {
            Btns[0]->IsSelected = true;
            if (ToolMain::Get()->IndexSelected != 5) {
                Btns.erase(Btns.begin() + 1, Btns.begin() + 4);
            }
        }
        setRect();
        win->Refresh();
    }
    return true;
}
void ToolSub::InitBtns(int mainToolSelectedIndex)
{
    Btns.clear();
    switch (mainToolSelectedIndex)
    {
    case 0: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形填充"));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 1: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::ellipseFill, L"椭圆填充"));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 2: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::arrowFill, L"箭头填充", false, true, 22, SK_ColorTRANSPARENT, true));
        addColorBtns();
        break;
    }
    case 3: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::numberFill, L"标号填充", false, true, 22, SK_ColorTRANSPARENT, true));
        addColorBtns();
        break;
    }
    case 4: {
        addStrokeWidthBtns(0);
        addColorBtns();
        break;
    }
    case 5: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::transparent, L"是否透明", false, true, 22, SK_ColorTRANSPARENT, true));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 6: {
        addColorBtns();
        break;
    }
    case 7: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形马赛克"));
        addStrokeWidthBtns(1);
        break;
    }
    case 8: {
        Btns.push_back(std::make_shared<ToolBtn>(Icon::rectFill, L"矩形橡皮擦"));
        addStrokeWidthBtns(1);
        break;
    }
    default:
        break;
    }
    setRect();
}
bool ToolSub::OnPaint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return false;
    }
    auto toolMain = ToolMain::Get();
    if (toolMain->IndexSelected < 0)
    {
        return false;
    }    
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawPath(p, paint);
    auto left{ ToolRect.fLeft };
    for (auto& btn : Btns)
    {
        btn->Paint(canvas, paint, left, ToolRect.fTop);
        left += ToolBtn::Width;
    }
    paint.setStroke(true);
    paint.setStrokeWidth(0.6f);
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    canvas->drawPath(p, paint);
    return false;
}
void ToolSub::setRect()
{
    auto toolMain = ToolMain::Get();
    auto width = Btns.size() * ToolBtn::Width;
    auto left = toolMain->ToolRect.left();
    auto top = toolMain->ToolRect.bottom() + MarginTop;
    auto mainToolBtnCenterPointX = left + toolMain->IndexSelected * ToolBtn::Width + ToolBtn::Width / 2;
    if (toolMain->IndexSelected > 5) {
        left = mainToolBtnCenterPointX - width / 2;
    }
    ToolRect.setXYWH(left, top, width, ToolBtn::Height);
    p.reset();
    p.moveTo(mainToolBtnCenterPointX, top - MarginTop / 3 * 2);  // 顶点
    p.lineTo(mainToolBtnCenterPointX - MarginTop, top);  // 左下角
    p.lineTo(left, top);
    p.lineTo(left, top + ToolBtn::Height);
    p.lineTo(left + width, top + ToolBtn::Height);
    p.lineTo(left + width, top);
    p.lineTo(mainToolBtnCenterPointX + MarginTop, top);  // 右下角
    p.close();
}
bool ToolSub::GetFill()
{
    return Btns[0]->IsSelected;
}
int ToolSub::GetStroke(int index)
{
    if (Btns[index]->IsSelected) {
        return 1;
    }
    else if (Btns[index+1]->IsSelected) {
        return 2;
    }
    else {
        return 3;
    }
}
SkColor ToolSub::GetColor()
{
    auto it = std::find_if(Btns.begin(), Btns.end(), [](auto& btn) {
        return btn->Icon == Icon::check;
        });
    if (it == Btns.end()) {
        return SK_ColorBLACK;
    }
    return it->get()->FontColor;
}
