#include "ToolSub.h"
#include "include/core/SkCanvas.h"
#include "../App.h"
#include "../WinBase.h"
#include "ToolMain.h"
#include "ToolBtn.h"

ToolSub::ToolSub()
{

}
ToolSub::~ToolSub()
{

}
void ToolSub::Init()
{
    listenLeftBtnDown(std::bind(&ToolSub::OnLeftBtnDown, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseMove(std::bind(&ToolSub::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2));
    listenCustomMsg(std::bind(&ToolSub::OnCustomMsg, this, std::placeholders::_1, std::placeholders::_2));
    listenPaint(std::bind(&ToolSub::OnPaint, this, std::placeholders::_1));
}
void ToolSub::OnCustomMsg(const EventType& type, const uint32_t& msg)
{
    if (type == EventType::showHideSubTool) {
        if (msg == 999999) {
            p.reset();
            Btns.clear();
        }
        else {
            InitBtns(msg);
            setRect();
        }
    }
}
void ToolSub::addStrokeWidthBtns(int index)
{
    ToolBtn btn1(16);
    btn1.fontSize = 22;
    ToolBtn btn2(16);
    btn2.fontSize = 52;
    ToolBtn btn3(16);
    btn3.fontSize = 82;
    Btns.insert(Btns.begin() + index, {btn1,btn2,btn3});
}
void ToolSub::addColorBtns()
{
    {
        ToolBtn btn(17);
        btn.fontColor = 0xFFcf1322;
        btn.isSelected = true;
        btn.info = L"红"; 
        Btns.push_back(btn);
    }
    { 
        ToolBtn btn(18);
        btn.fontColor = 0xFFd48806;
        btn.info = L"黄";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFF389e0d;
        btn.info = L"绿";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFF13c2c2;
        btn.info = L"青";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFF0958d9;
        btn.info = L"蓝";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFF722ed1;
        btn.info = L"紫";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFFeb2f96;
        btn.info = L"粉";
        Btns.push_back(btn);
    }
    {
        ToolBtn btn(18);
        btn.fontColor = 0xFF000000;
        btn.info = L"黑";
        Btns.push_back(btn);
    }
}
void ToolSub::OnLeftBtnDown(const int& x, const int& y)
{
    //isMouseDown = true;
    //auto win = App::GetWin();
    //if (win->state < State::tool)
    //{
    //    return false;
    //}
    //if (!toolRect.contains(x, y))
    //{
    //    return false;
    //}
    //Recorder::Get()->ProcessText();
    //win->IsMouseDown = false;
    //int index = (x - toolRect.left()) / ToolBtn::Width;
    //if (Btns[index]->Icon == Icon::dot) {
    //    if (Btns[index]->IsSelected) {
    //        return true;
    //    }
    //    for (auto& btn : Btns)
    //    {
    //        if (btn->IsSelected && btn->Icon == Icon::dot)
    //        {
    //            btn->IsSelected = false;
    //            break;
    //        }
    //    }
    //    Btns[index]->IsSelected = true;
    //    setRect();
    //    win->Refresh();
    //    return true;
    //}
    //else if (Btns[index]->Icon == Icon::uncheck) {
    //    for (auto& btn : Btns)
    //    {
    //        if (btn->Icon == Icon::check)
    //        {
    //            btn->IsSelected = false;
    //            btn->Icon = Icon::uncheck;
    //            break;
    //        }
    //    }
    //    Btns[index]->Icon = Icon::check;
    //    Btns[index]->IsSelected = true;
    //    setRect();
    //    win->Refresh();
    //    return true;
    //}
    //if (index == 0) {
    //    if (Btns[0]->IsSelected) {
    //        Btns[0]->IsSelected = false;
    //        if (ToolMain::Get()->IndexSelected != 5) { //transparen line
    //            addStrokeWidthBtns(1);
    //        }
    //    }
    //    else {
    //        Btns[0]->IsSelected = true;
    //        if (ToolMain::Get()->IndexSelected != 5) {
    //            Btns.erase(Btns.begin() + 1, Btns.begin() + 4);
    //        }
    //    }
    //    setRect();
    //    win->Refresh();
    //}
    //return true;
}
void ToolSub::InitBtns(const int& mainBtnId)
{
    Btns.clear();
    switch (mainBtnId)
    {
    case 0: {
        ToolBtn btn(15);
        btn.info = L"矩形填充";
        Btns.push_back(std::move(btn));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 1: {
        ToolBtn btn(19);
        btn.info = L"椭圆填充";
        Btns.push_back(std::move(btn));
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 2: {
        ToolBtn btn(20);
        btn.info = L"箭头填充";
        Btns.push_back(std::move(btn));
        addColorBtns();
        break;
    }
    case 3: {
        ToolBtn btn(21);
        btn.info = L"标号填充";
        Btns.push_back(btn);
        addColorBtns();
        break;
    }
    case 4: {
        addStrokeWidthBtns(0);
        addColorBtns();
        break;
    }
    case 5: {
        ToolBtn btn(22);
        btn.info = L"是否透明";
        Btns.push_back(btn);
        addStrokeWidthBtns(1);
        addColorBtns();
        break;
    }
    case 6: {
        addColorBtns();
        break;
    }
    case 7: {
        ToolBtn btn(15);
        btn.info = L"矩形马赛克";
        Btns.push_back(btn);
        addStrokeWidthBtns(1);
        break;
    }
    case 8: {
        ToolBtn btn(15);
        btn.info = L"矩形橡皮擦";
        Btns.push_back(btn);
        addStrokeWidthBtns(1);
        break;
    }
    default:
        break;
    }
    setRect();
}
void ToolSub::OnPaint(SkCanvas* canvas)
{
    auto win = App::GetWin();
    if (win->state < State::tool && Btns.size()>0)
    {
        return;
    }
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setAntiAlias(true);
    canvas->drawPath(p, paint);
    auto left{ toolRect.fLeft };
    for (auto& btn : Btns)
    {
        btn.Paint(canvas, paint, left, toolRect.fTop);
        left += ToolBtn::Width;
    }
    paint.setStroke(true);
    paint.setStrokeWidth(0.6f);
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    canvas->drawPath(p, paint);
}
void ToolSub::setRect()
{
    auto toolMain = App::GetWin()->toolMain.get();
    auto width = Btns.size() * ToolBtn::Width;
    auto left = toolMain->toolRect.left();
    auto top = toolMain->toolRect.bottom() + MarginTop;
    auto mainToolBtnCenterPointX = left + toolMain->indexSelected * ToolBtn::Width + ToolBtn::Width / 2;
    if (toolMain->indexSelected > 5) {
        left = mainToolBtnCenterPointX - width / 2;
    }
    toolRect.setXYWH(left, top, width, ToolBtn::Height);
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
    return Btns[0].isSelected;
}
int ToolSub::GetStroke(int index)
{
    if (Btns[index].isSelected) {
        return 1;
    }
    else if (Btns[index + 1].isSelected) {
        return 2;
    }
    else {
        return 3;
    }
    return 0;
}
SkColor ToolSub::GetColor()
{
    auto it = std::find_if(Btns.begin(), Btns.end(), [](auto& btn) {
        return btn.iconCode == (const char*)u8"\ue721";
        });
    if (it == Btns.end()) {
        return SK_ColorBLACK;
    }
    return it->fontColor;
}
