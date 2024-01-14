#include <Windows.h>
#include <shobjidl.h>
#include <format>
#include "ToolMain.h"
#include "State.h"
#include "CutMask.h"
#include "App.h"
#include "include/core/SkTextBlob.h"
#include "Icon.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "WindowPin.h"

ToolMain *toolMain;


ToolMain::~ToolMain()
{
    toolMain = nullptr;
}

ToolMain::ToolMain()
{

}
void ToolMain::saveFile()
{
    IFileOpenDialog* dialog;
    CLSID param1 = CLSID_FileSaveDialog, param2 = IID_IFileSaveDialog;
    auto hr = CoCreateInstance(param1, NULL, CLSCTX_ALL, param2, reinterpret_cast<void**>(&dialog));
    if (FAILED(hr))
    {
        MessageBox(NULL, L"Failed to create COM FileSaveDialog object.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    COMDLG_FILTERSPEC FileTypes[] = {{ L"All Pictures", L"*.png;" },{ L"All files", L"*.*" }};
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    std::wstring name = std::format(L"{}{}{}{}{}{}{}", localTime.wYear, localTime.wMonth, localTime.wDay,
        localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
    dialog->SetFileName(name.c_str());
    dialog->SetFileTypes(2, FileTypes);
    dialog->SetTitle(L"Save File");
    dialog->SetDefaultExtension(L"png");
    dialog->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT);
    hr = dialog->Show(App::GetWin()->hwnd);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) { //用户取消
        dialog->Release();
        return;
    }
    IShellItem* pItem;
    hr = dialog->GetResult(&pItem);
    if (FAILED(hr))
    {
        dialog->Release();
        MessageBox(NULL, L"Failed to get file path from save dialog.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    PWSTR filePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    if (FAILED(hr))
    {
        CoTaskMemFree(filePath);
        dialog->Release();
        MessageBox(NULL, L"Failed to get file name from save dialog.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    std::wstringstream ss;
    ss << filePath;
    CoTaskMemFree(filePath);
    dialog->Release();
    std::wstring wstr = ss.str();
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    App::GetWin()->Save(str);
}
void ToolMain::SetPositionByCutMask()
{
    auto mask = CutMask::Get();
    float left{ mask->CutRect.fRight - btns.size() * ToolBtn::Width };
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
    ToolRect.setXYWH(left,top, btns.size() * ToolBtn::Width, ToolBtn::Height);
}
void ToolMain::SetPosition(const float& x, const float& y)
{
    ToolRect.setXYWH(x, y, btns.size() * ToolBtn::Width, ToolBtn::Height);
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
    win->IsMouseDown = false; //不然在主工具栏上拖拽的时候，会改变CutBox，而且改变完CutBox后不会在显示工具栏
    if (IndexHovered == IndexSelected)
    {
        btns[IndexHovered]->IsSelected = false;
        IndexSelected = -1;
        win->state = State::tool;
        if (topFlag) {
            ToolRect.offset(0, MarginTop + ToolBtn::Height);
        }
        win->Refresh();
    }
    else
    {
        if (btns[IndexHovered]->Selectable) {
            btns[IndexHovered]->IsSelected = true;
            if (IndexSelected >= 0) {
                btns[IndexSelected]->IsSelected = false;
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
            if (btns[IndexHovered]->IsDisable) {
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
                saveFile();
                btns[12]->IsHover = false;
                break;
            }
            case 13: {
                App::GetWin()->SaveToClipboard();                
                btns[13]->IsHover = false;
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
    for (auto& btn : btns)
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
    btns[9]->IsDisable = flag;
}

void ToolMain::SetRedoDisable(bool flag)
{
    btns[10]->IsDisable = flag;
}

void ToolMain::InitBtns()
{
    bool flag = btns.size() > 0; //不是第一次
    btns.clear();
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
    btns.push_back(std::make_shared<ToolBtn>(Icon::undo, L"上一步", true, false)); //9
    btns.push_back(std::make_shared<ToolBtn>(Icon::redo, L"下一步", true, false)); //10
    btns.push_back(std::make_shared<ToolBtn>(Icon::pin, L"钉住截图区", flag, false));//11
    btns.push_back(std::make_shared<ToolBtn>(Icon::save, L"保存为文件", false, false));//12
    btns.push_back(std::make_shared<ToolBtn>(Icon::copy, L"保存到剪切板", false, false));//13
    btns.push_back(std::make_shared<ToolBtn>(Icon::close, L"退出", false, false));//14
}

void ToolMain::Reset()
{
    IndexSelected = -1;
    IndexHovered = -1;
    InitBtns();
    SetPositionByCutMask();
}


