#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "WindowMain.h"
#include "AppFont.h"
#include "include/core/SkTextBlob.h"
#include "Icon.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "Screen.h"

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
    btns.push_back(std::make_shared<ToolBtn>(Icon::undo, L"上一步",true,false)); //9
    btns.push_back(std::make_shared<ToolBtn>(Icon::redo, L"下一步",true,false)); //10
    btns.push_back(std::make_shared<ToolBtn>(Icon::pin, L"钉住截图区",false,false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::save, L"保存", false, false));
    btns.push_back(std::make_shared<ToolBtn>(Icon::close, L"退出", false, false));
}

void ToolMain::setPosition()
{
    auto mask = CutMask::get();
    float left{ mask->CutRect.fRight - btns.size() * ToolBtn::width };
    float top{ mask->CutRect.fBottom + MarginTop };
    //三个缝隙高度和两个工具条高度
    auto heightSpan = MarginTop * 3 + ToolBtn::height * 2;    
    auto screen = Screen::GetScreen(mask->CutRect.fRight, mask->CutRect.fBottom + heightSpan);
    if (screen) { //工具条右下角在屏幕内
        //工具条左上角不在屏幕内
        if (!Screen::GetScreen(left, top)) {
            left = screen->fLeft;
        }
    }
    else { //工具条右下角不在屏幕内
        //判断屏幕顶部是否有足够的空间，工具条是否可以在CutRect右上角
        screen = Screen::GetScreen(mask->CutRect.fRight, mask->CutRect.fTop - heightSpan);
        if (screen) { //工具条右上角在屏幕内  
            if (Screen::GetScreen(left, mask->CutRect.fTop - heightSpan)) { //工具条左上角在屏幕内
                if (IndexSelected == -1) { //不需要显示子工具条，主工具条贴着截图区                    
                    top = mask->CutRect.fTop - MarginTop - ToolBtn::height;
                }
                else { //需要显示子工具条，要为子工具条留出区域
                    top = mask->CutRect.fTop - MarginTop * 2 - ToolBtn::height * 2;
                }
            }
            else { //工具条左上角不在屏幕中
                left = screen->fLeft;
                if (IndexSelected == -1) { //不需要显示子工具条，主工具条贴着截图区
                    top = mask->CutRect.fTop - MarginTop - ToolBtn::height;
                }
                else { //需要显示子工具条，要为子工具条留出区域
                    top = mask->CutRect.fTop - MarginTop * 2 - ToolBtn::height * 2;
                }
            }
        }
        else { //工具条右上角不在屏幕内，此时屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内            
            if (Screen::GetScreen(left, mask->CutRect.fBottom - heightSpan)) { //工具条左上角在屏幕内
                if (IndexSelected == -1) { //不需要显示子工具条，主工具条贴着截图区
                    top = mask->CutRect.fBottom - MarginTop - ToolBtn::height;
                }
                else { //需要显示子工具条，要为子工具条留出区域
                    top = mask->CutRect.fBottom - MarginTop * 2 - ToolBtn::height * 2;
                }
            }
            else { //工具条左上角不在屏幕中，得到截图区域所在屏幕
                screen = Screen::GetScreen(mask->CutRect.fRight, mask->CutRect.fBottom);
                if (screen) {
                    left = screen->fLeft;
                    if (IndexSelected == -1) { //不需要显示子工具条，主工具条贴着截图区
                        top = mask->CutRect.fBottom - MarginTop - ToolBtn::height;
                    }
                    else { //需要显示子工具条，要为子工具条留出区域
                        top = mask->CutRect.fBottom - MarginTop * 2 - ToolBtn::height * 2;
                    }
                }
            }
        }
    }
    ToolRect.setXYWH(left,top, btns.size() * ToolBtn::width, ToolBtn::height);
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
    auto win = WindowMain::get();
    if (win->state < State::tool)
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
        win->state = State::tool;
        win->Refresh();
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
            win->state = (State)(IndexSelected + 3);
            win->Refresh();
        }
        else {
            if (btns[IndexHovered]->isDisable) {
                return true;
            }
            switch (IndexHovered)
            {
            case 9: {
                Recorder::get()->undo();
                break;
            }
            case 10: {
                Recorder::get()->redo();
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
    auto winMain = WindowMain::get();
    if (winMain->state < State::tool)
    {
        return false;
    }
    setPosition();
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

void ToolMain::setUndoDisable(bool flag)
{
    btns[9]->isDisable = flag;
}

void ToolMain::setRedoDisable(bool flag)
{
    btns[10]->isDisable = flag;
}


