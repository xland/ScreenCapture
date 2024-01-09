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
#include "Screen.h"
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

void ToolMain::SetPosition(const float& x, const float& y)
{
    ToolRect.setXYWH(x, y, btns.size() * ToolBtn::width, ToolBtn::height);
}


void ToolMain::Init()
{
    toolMain = new ToolMain();
}

ToolMain *ToolMain::get()
{
    return toolMain;
}

bool ToolMain::OnMouseDown(const int& x, const int& y)
{
    auto win = App::GetWin();
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
                Recorder::get()->Undo();
                break;
            }
            case 10: {
                Recorder::get()->Redo();
                break;
            }
            case 11: {
                btns[11]->isHover = false;
                btns[11]->isDisable = true;
                App::Pin();
                break;
            }
            case 12: {
                saveFile();
                btns[12]->isHover = false;
                break;
            }
            case 13: {
                App::GetWin()->SaveToClipboard();                
                btns[13]->isHover = false;
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
        x += ToolBtn::width;
    }
    paint.setColor(SkColorSetARGB(255, 180, 180, 180));
    x = ToolRect.left()+ToolBtn::width*9;
    paint.setStroke(true);
    paint.setStrokeWidth(0.6f);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom()-12), paint);
    x += ToolBtn::width * 2;
    canvas->drawLine(SkPoint::Make(x, y + 12), SkPoint::Make(x, ToolRect.bottom() - 12), paint);    
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    canvas->drawRect(ToolRect,paint);
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

void ToolMain::InitBtns()
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
    btns.push_back(std::make_shared<ToolBtn>(Icon::undo, L"上一步", true, false)); //9
    btns.push_back(std::make_shared<ToolBtn>(Icon::redo, L"下一步", true, false)); //10
    btns.push_back(std::make_shared<ToolBtn>(Icon::pin, L"钉住截图区", false, false));//11
    btns.push_back(std::make_shared<ToolBtn>(Icon::save, L"保存为文件", false, false));//12
    btns.push_back(std::make_shared<ToolBtn>(Icon::copy, L"保存到剪切板", false, false));//13
    btns.push_back(std::make_shared<ToolBtn>(Icon::close, L"退出", false, false));//14
}


