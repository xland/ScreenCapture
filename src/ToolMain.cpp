#include "ToolMain.h"
#include <format>
#include <string>
#include "include/core/SkTextBlob.h"
#include "State.h"
#include "CutMask.h"
#include "App.h"
#include "Icon.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "WindowPin.h"
#include "Lang.h"

ToolMain *toolMain;


ToolMain::~ToolMain()
{
    toolMain = nullptr;
}

ToolMain::ToolMain()
{
    InitBtns();
}
void ToolMain::SetPositionByCutMask()
{
    auto mask = CutMask::Get();
    float left{ mask->CutRect.fRight - Btns.size() * ToolBtn::Width };
    float top{ mask->CutRect.fBottom + MarginTop };
    //三个缝隙高度和两个工具条高度
    auto heightSpan = MarginTop * 3 + ToolBtn::Height * 2;    
    auto screen = App::GetScreen(mask->CutRect.fRight, mask->CutRect.fBottom + heightSpan);
    if (screen) { //工具条右下角在屏幕内
        topFlag = false;
        //工具条左上角不在屏幕内
        if (!App::GetScreen(left, top)) {
            left = screen->fLeft;
        }
    }
    else { //工具条右下角不在屏幕内
        topFlag = true;
        //判断屏幕顶部是否有足够的空间，工具条是否可以在CutRect右上角
        screen = App::GetScreen(mask->CutRect.fRight, mask->CutRect.fTop - heightSpan);
        if (screen) { //工具条右上角在屏幕内  
            if (App::GetScreen(left, mask->CutRect.fTop - heightSpan)) { //工具条左上角在屏幕内
                top = mask->CutRect.fTop - MarginTop - ToolBtn::Height;
            }
            else { //工具条左上角不在屏幕中
                left = screen->fLeft;
                top = mask->CutRect.fTop - MarginTop - ToolBtn::Height;
            }
        }
        else { //工具条右上角不在屏幕内，此时屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内            
            if (App::GetScreen(left, mask->CutRect.fBottom - heightSpan)) { //工具条左上角在屏幕内
                top = mask->CutRect.fBottom - MarginTop - ToolBtn::Height;
            }
            else { //工具条左上角不在屏幕中，得到截图区域所在屏幕
                screen = App::GetScreen(mask->CutRect.fRight, mask->CutRect.fBottom);
                if (screen) {
                    left = screen->fLeft;
                    top = mask->CutRect.fBottom - MarginTop - ToolBtn::Height;
                }
            }
        }
    }
    ToolRect.setXYWH(left,top, Btns.size() * ToolBtn::Width, ToolBtn::Height);
}
void ToolMain::SetPosition(const float& x, const float& y)
{
    ToolRect.setXYWH(x, y, Btns.size() * ToolBtn::Width, ToolBtn::Height);
}
void ToolMain::Init()
{
    toolMain = new ToolMain();
}
ToolMain *ToolMain::Get()
{
    return toolMain;
}

bool ToolMain::OnMouseDown(const int& x, const int& y)
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
    Recorder::Get()->ProcessText();
    win->IsMouseDown = false; //不然在主工具栏上拖拽的时候，会改变CutBox，而且改变完CutBox后不会在显示工具栏
    if (IndexHovered == IndexSelected)
    {
        Btns[IndexHovered]->IsSelected = false;
        IndexSelected = -1;
        win->state = State::tool;
        if (topFlag) {
            ToolRect.offset(0, MarginTop + ToolBtn::Height);
        }
        win->Refresh();
    }
    else
    {
        if (Btns[IndexHovered]->Selectable) {
            Btns[IndexHovered]->IsSelected = true;
            if (IndexSelected >= 0) {
                Btns[IndexSelected]->IsSelected = false;
            }
            else {
                if (topFlag) {
                    ToolRect.offset(0, 0 - MarginTop - ToolBtn::Height);
                }
            }
            IndexSelected = IndexHovered;
            ToolSub::Get()->InitBtns(IndexSelected);
            win->state = (State)(IndexSelected + 3);
            win->Refresh();
        }
        else {
            if (Btns[IndexHovered]->IsDisable) {
                return true;
            }
            switch (IndexHovered)
            {
            case 9: { //上一步
                Recorder::Get()->Undo();
                break;
            }
            case 10: { //下一步
                Recorder::Get()->Redo();
                break;
            }
            case 11: {
                App::Pin();
                break;
            }
            case 12: {
                App::SaveFile();
                Btns[12]->IsHover = false;
                break;
            }
            case 13: {
                App::GetWin()->SaveToClipboard();                
                Btns[13]->IsHover = false;
                break;
            }
            case 14: {
                App::Quit(1);
                break;
            }
            default:
                break;
            }
        }
    }
    return true;
}
bool ToolMain::OnPaint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return false;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    canvas->drawRect(ToolRect,paint);
    auto x = ToolRect.left();
    auto y = ToolRect.top();
    for (auto& btn : Btns)
    {
        btn->Paint(canvas, paint, x, y);
        x += ToolBtn::Width;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x = ToolRect.left()+ToolBtn::Width*9;
    paint.setStroke(true);
    paint.setStrokeWidth(0.6f);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom()-12), paint);
    x += ToolBtn::Width * 2;
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom() - 12), paint);    
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    canvas->drawRect(ToolRect,paint);
    return false;
}

void ToolMain::SetUndoDisable(bool flag)
{
    Btns[9]->IsDisable = flag;
}

void ToolMain::SetRedoDisable(bool flag)
{
    Btns[10]->IsDisable = flag;
}

void ToolMain::InitBtns()
{
    Btns.push_back(std::make_shared<ToolBtn>(Icon::rect, Lang::Get(Lang::Key::BtnRect)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::ellipse, Lang::Get(Lang::Key::BtnEllipse)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::arrow, Lang::Get(Lang::Key::BtnArrow)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::number, Lang::Get(Lang::Key::BtnNumber)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::pen, Lang::Get(Lang::Key::BtnPen)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::line, Lang::Get(Lang::Key::BtnLine)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::text, Lang::Get(Lang::Key::BtnText)));
    //Btns.push_back(std::make_shared<ToolBtn>(Icon::image, L"图片"));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::mosaic, Lang::Get(Lang::Key::BtnMosaic)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::eraser, Lang::Get(Lang::Key::BtnEraser)));
    Btns.push_back(std::make_shared<ToolBtn>(Icon::undo, Lang::Get(Lang::Key::BtnUndo), true, false)); //9
    Btns.push_back(std::make_shared<ToolBtn>(Icon::redo, Lang::Get(Lang::Key::BtnRedo), true, false)); //10
    Btns.push_back(std::make_shared<ToolBtn>(Icon::pin, Lang::Get(Lang::Key::BtnPin), false, false));//11
    Btns.push_back(std::make_shared<ToolBtn>(Icon::save, Lang::Get(Lang::Key::BtnSave), false, false));//12
    Btns.push_back(std::make_shared<ToolBtn>(Icon::copy, Lang::Get(Lang::Key::BtnCopy), false, false));//13
    Btns.push_back(std::make_shared<ToolBtn>(Icon::close, Lang::Get(Lang::Key::BtnClose), false, false));//14
}

void ToolMain::UnSelectAndHoverAll()
{
    IndexSelected = -1;
    IndexHovered = -1;
    for (auto& btn : Btns) {
        btn->IsHover = false;
        btn->IsSelected = false;
    }
}


