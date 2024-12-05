#include "App.h"
#include <Windows.h>
#include <shlobj.h>
#include <sstream>
#include <filesystem>
#include <include/core/SkFont.h>
#include <include/core//SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/core/SkData.h>
#include <include/ports/SkTypeface_win.h>
#include "Resource/res.h"
#include "Layer/WinFull.h"
#include "Lang.h"


SkFont* fontIcon{ nullptr };
SkFont* fontText{ nullptr };
WindowBase* win { nullptr };
static int exitCode{ 0 };
static std::vector<std::shared_ptr<SkRect>> screens;
sk_sp<SkFontMgr> fontMgr;
std::wstring savePath;

App::~App()
{
}

void App::Init(std::wstring&& cmd)
{
    Lang::Init(cmd);
    auto pos = cmd.find(L"--pin:clipboard");
    if (pos != std::wstring::npos) {
        pinClipboard();
    }
    else {
        createWindow();
    }
    pos = cmd.find(L"--dir:\"");
    if (pos != std::wstring::npos) {
        pos += 7; //'--dir:' 6
        size_t endPos = cmd.find(L'\"', pos);
        savePath = cmd.substr(pos, endPos - pos);
    }
}

void App::Dispose()
{
}

void App::createWindow() {
    //win = new WindowMain();
    //ToolMain::Init();
    //ToolSub::Init();
    //Recorder::Init();
    //CutMask::Get()->EnumWinRects();
    //win->Show();
    //Cursor::Cross();
    //initScreens();
}
void App::pinClipboard() {
    //if (IsClipboardFormatAvailable(CF_BITMAP) || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_DIBV5))
    //{
    //    win = new WindowPin(true);
    //    ToolMain::Init();
    //    auto tm = ToolMain::Get();
    //    tm->Btns[11]->IsDisable = true;
    //    auto tmW = tm->ToolRect.width();
    //    tm->ToolRect.setXYWH(win->w - tmW, win->h - ToolBtn::Height - ToolBase::MarginTop, tmW, ToolBtn::Height);
    //    ToolSub::Init();
    //    Recorder::Init();        
    //    win->Show();
    //    Cursor::Arrow();
    //}
    //else
    //{
    //    createWindow();
    //}
}


WindowBase* App::GetWin()
{
	return win;
}

SkFont* App::GetFontIcon()
{
	return fontIcon;
}

SkFont* App::GetFontText()
{
	return fontText;
}

int App::GetExitCode() {
    return exitCode;
}

void App::Pin()
{
    //auto pinWin = new WindowPin();
    //pinWin->Show();
    //Cursor::Arrow();
    //win->Close(0);
    //delete win;
    //win = pinWin;
    //Recorder::Get()->Reset();
    //screens.clear();
}

void App::Quit(const int& code) {
    //exitCode = code;
    //win->Close(0);
    //delete win;
    //PostQuitMessage(0);
}
void App::initScreens() {
    //EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
    //    {
    //        MONITORINFO info;
    //        info.cbSize = sizeof(MONITORINFO);
    //        GetMonitorInfo(hMonitor, &info);
    //        POINT leftTop{ .x{info.rcMonitor.left},.y{info.rcMonitor.top} };
    //        POINT rightBottom{ .x{info.rcMonitor.right},.y{info.rcMonitor.bottom} };
    //        auto hwnd = App::GetWin()->hwnd;
    //        ScreenToClient(hwnd, &leftTop);
    //        ScreenToClient(hwnd, &rightBottom);
    //        auto ptr = std::make_shared<SkRect>(SkRect::MakeLTRB(leftTop.x, leftTop.y, rightBottom.x + 1, rightBottom.y + 1));
    //        screens.push_back(ptr);
    //        return TRUE;
    //    }, NULL);
}

void App::SaveFile() {
    //if (!savePath.empty() && std::filesystem::exists(savePath) && std::filesystem::is_directory(savePath)) {        
    //    auto filePath = std::filesystem::path{ savePath } / App::createFileName();
    //    auto filePathStr = App::toStdString(filePath.wstring());
    //    win->Save(filePathStr);
    //    return;
    //}
    //IFileSaveDialog* dialog;    
    //auto hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
    //if (FAILED(hr))
    //{
    //    MessageBox(NULL, L"Failed to create COM FileSaveDialog object.", L"Error", MB_OK | MB_ICONERROR);
    //    return;
    //}
    //COMDLG_FILTERSPEC FileTypes[] = { { L"All Pictures", L"*.png;" },{ L"All files", L"*.*" } };
    //std::wstring name = App::createFileName();
    //dialog->SetFileName(name.c_str());
    //dialog->SetFileTypes(2, FileTypes);
    //dialog->SetTitle(L"Save File");
    //dialog->SetDefaultExtension(L"png");
    //dialog->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT);
    //hr = dialog->Show(App::GetWin()->hwnd);
    //if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) { //用户取消
    //    dialog->Release();
    //    return;
    //}
    //IShellItem* pItem;
    //hr = dialog->GetResult(&pItem);
    //if (FAILED(hr))
    //{
    //    dialog->Release();
    //    MessageBox(NULL, L"Failed to get file path from save dialog.", L"Error", MB_OK | MB_ICONERROR);
    //    return;
    //}
    //PWSTR filePath;
    //hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    //if (FAILED(hr))
    //{
    //    CoTaskMemFree(filePath);
    //    dialog->Release();
    //    MessageBox(NULL, L"Failed to get file name from save dialog.", L"Error", MB_OK | MB_ICONERROR);
    //    return;
    //}
    //std::wstringstream ss;
    //ss << filePath;
    //CoTaskMemFree(filePath);
    //dialog->Release();
    //auto filePathStr = App::toStdString(ss.str());
    //win->Save(filePathStr);
}

std::shared_ptr<SkRect> App::GetScreen(const float& x, const float& y)
{
    std::shared_ptr<SkRect> result;
    for (size_t i = 0; i < screens.size(); i++)
    {
        if (screens[i]->contains(x, y)) {
            result = screens[i];
            break;
        }
    }
    return result;
}
void App::Log(std::string&& info) {
    SkDebugf(info.data());
}