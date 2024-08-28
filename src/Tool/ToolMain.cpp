#include "ToolMain.h"
#include <format>
#include <string>
#include <ranges>
#include <cmath>
#include <vector>
#include "include/core/SkTextBlob.h"
#include "../CutMask.h"
#include "../WinMax.h"
#include "ToolSub.h"
#include "../Lang.h"
#include "../Screen.h"
#include "../Cmd.h"
#include "../App.h"

ToolMain::ToolMain()
{
}
ToolMain::~ToolMain()
{
}

void ToolMain::Init()
{
    initBtns();
    listenLeftBtnDown(std::bind(&ToolMain::OnLeftBtnDown, this, std::placeholders::_1, std::placeholders::_2));
    listenLeftBtnUp(std::bind(&ToolMain::OnLeftBtnUp, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseMove(std::bind(&ToolMain::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseDrag(std::bind(&ToolMain::OnMouseDrag, this, std::placeholders::_1, std::placeholders::_2));
    listenCustomMsg(std::bind(&ToolMain::OnCustomMsg, this, std::placeholders::_1, std::placeholders::_2));
    listenPaint(std::bind(&ToolMain::OnPaint, this, std::placeholders::_1));
}

void ToolMain::OnCustomMsg(const EventType& type, const uint32_t& msg)
{
    if (type == EventType::maskReady) {
        setPositionByCutMask();        
    }
    else if (type == EventType::undoDisable) {
        Btns[9].isDisable = msg;
    }
    else if (type == EventType::redoDisable) {
        Btns[10].isDisable = msg;
    }
    else if (type == EventType::unselectAllTool) {
        indexSelected = -1;
        indexHovered = -1;
        for (auto& btn : Btns) {
            btn.isHover = false;
            btn.isSelected = false;
        }
    }
}

void ToolMain::initBtns()
{
    auto cmd = Cmd::Get();
    auto val = cmd->GetVal(L"tool");
    if (val.empty()) {
        for (size_t i = 0; i < 14; i++)
        {
            Btns.push_back(ToolBtn(i));
            spliters.push_back(9);
            spliters.push_back(11);
        }
    }
    else {
        auto splitView = std::ranges::views::split(val, ',');
        std::vector<int> result;
        auto spliterIndex{ 0 };
        for (auto part : splitView) {
            std::wstring_view partStr(part.begin(), part.end());
            auto i = std::stoi(std::wstring(partStr));
            if (i == -1) {
                spliters.push_back(spliterIndex);
            }
            else {
                Btns.push_back(ToolBtn(i));
            }            
        }
    }
}

void ToolMain::setPositionByCutMask()
{
    auto win = static_cast<WinMax*>(App::GetWin());
    auto mask = win->cutMask.get();
    auto screen = win->screen.get();
    float left{ mask->cutRect.fRight - Btns.size() * ToolBtn::Width };
    float top{ mask->cutRect.fBottom + MarginTop };
    //三个缝隙高度和两个工具条高度
    auto heightSpan = MarginTop * 3 + ToolBtn::Height * 2;
    auto screenLeft = screen->GetScreenLeftByPos(mask->cutRect.fRight, mask->cutRect.fBottom + heightSpan);
    if (!std::isnan(screenLeft)) { //工具条右下角在屏幕内，不是NAN
        topFlag = false;
        //工具条左上角不在屏幕内，是NAN
        if (std::isnan(screen->GetScreenLeftByPos(left, top))) {
            left = screenLeft;
        }
    }
    else { //工具条右下角不在屏幕内
        topFlag = true;
        //判断屏幕顶部是否有足够的空间，工具条是否可以在cutRect右上角
        screenLeft = screen->GetScreenLeftByPos(mask->cutRect.fRight, mask->cutRect.fTop - heightSpan);
        if (!std::isnan(screenLeft)) { //工具条右上角在屏幕内  
            if (!std::isnan(screen->GetScreenLeftByPos(left, mask->cutRect.fTop - heightSpan))) { //工具条左上角在屏幕内
                top = mask->cutRect.fTop - MarginTop - ToolBtn::Height;
            }
            else { //工具条左上角不在屏幕中
                left = screenLeft;
                top = mask->cutRect.fTop - MarginTop - ToolBtn::Height;
            }
        }
        else { //工具条右上角不在屏幕内，此时屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内            
            if (!std::isnan(screen->GetScreenLeftByPos(left, mask->cutRect.fBottom - heightSpan))) { //工具条左上角在屏幕内
                top = mask->cutRect.fBottom - MarginTop - ToolBtn::Height;
            }
            else { //工具条左上角不在屏幕中，得到截图区域所在屏幕
                screenLeft = screen->GetScreenLeftByPos(mask->cutRect.fRight, mask->cutRect.fBottom);
                if (screen) {
                    left = screenLeft;
                    top = mask->cutRect.fBottom - MarginTop - ToolBtn::Height;
                }
            }
        }
    }
    toolRect.setXYWH(left, top, Btns.size() * ToolBtn::Width, ToolBtn::Height);
    win->Refresh();
}
void ToolMain::SetPosition(const float& x, const float& y)
{
    toolRect.setXYWH(x, y, Btns.size() * ToolBtn::Width, ToolBtn::Height);
}
void ToolMain::OnLeftBtnDown(const int& x, const int& y)
{
    isMouseDown = true;
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return; 
    }
    if (!toolRect.contains(x, y))
    {
        return;  
    }
    //Recorder::Get()->ProcessText();
    //win->IsMouseDown = false; //不然在主工具栏上拖拽的时候，会改变CutBox，而且改变完CutBox后不会在显示工具栏
    if (indexHovered == indexSelected)
    {
        Btns[indexHovered].isSelected = false;
        indexSelected = -1;
        win->state = State::tool;
        if (topFlag) {
            toolRect.offset(0, MarginTop + ToolBtn::Height);
        }
        win->Refresh();
    }
    else
    {
        if (Btns[indexHovered].selectable) {
            Btns[indexHovered].isSelected = true;
            if (indexSelected >= 0) {
                Btns[indexSelected].isSelected = false;
            }
            else {
                if (topFlag) {
                    toolRect.offset(0, 0 - MarginTop - ToolBtn::Height);
                }
            }
            indexSelected = indexHovered;
            //ToolSub::Get()->InitBtns(indexSelected);
            win->state = (State)(indexSelected + 3);
            win->Refresh();
        }
        else {
            if (Btns[indexHovered].isDisable) {
                return;
            }
            switch (indexHovered)
            {
            case 9: { //上一步
                //Recorder::Get()->Undo();
                break;
            }
            case 10: { //下一步
                //Recorder::Get()->Redo();
                break;
            }
            case 11: {
                //App::Pin();
                break;
            }
            case 12: {
                //App::SaveFile();
                Btns[12].isHover = false;
                break;
            }
            case 13: {
                //App::GetWin()->SaveToClipboard();
                Btns[13].isHover = false;
                break;
            }
            case 14: {
                //App::Quit(1);
                break;
            }
            default:
                break;
            }
        }
    }
}
void ToolMain::OnPaint(SkCanvas* canvas)
{
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    canvas->drawRect(toolRect, paint);
    auto x = toolRect.left();
    auto y = toolRect.top();
    for (auto& btn : Btns)
    {
        btn.Paint(canvas, paint, x, y);
        x += ToolBtn::Width;
    }
    paint.setColor(0xFFB4B4B4);
    paint.setStroke(true);
    paint.setStrokeWidth(0.6f);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    for (size_t i = 0; i < spliters.size(); i++)
    {
        x = toolRect.left() + ToolBtn::Width * spliters[i];
        canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, toolRect.bottom() - 12), paint);//undo redo spliter
    }
    paint.setColor(0xFF1676ff);
    canvas->drawRect(toolRect, paint);//绘制边框
}


