#include "App.h"
#include <Windows.h>
#include <shlobj.h>
#include "../res/res.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkData.h"
#include "WindowMain.h"
#include "WindowPin.h"
#include "Cursor.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "CutMask.h"
#include "Shape/ShapeDragger.h"


SkFont* fontIcon{ nullptr };
SkFont* fontText{ nullptr };
WindowBase* win { nullptr };
static int exitCode{ 0 };
static std::vector<std::shared_ptr<SkRect>> screens;

App::~App()
{
}

void App::Init()
{
    initFontText();
    initFontIcon();
    ToolMain::Init();
    ToolSub::Init();
    Recorder::Init();
    ShapeDragger::Init();
    Timer::Init();
    win = new WindowMain();
    CutMask::get()->EnumWinRects();
    win->Show();
    Cursor::Cross();
    initScreens();
}

void App::Dispose()
{
    screens.clear();
    delete Timer::get();
    delete ShapeDragger::get();
    delete Recorder::get();
    delete ToolSub::get();
    delete ToolMain::get();
    delete fontIcon;
    delete fontText;
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
    auto pinWin = new WindowPin();
    pinWin->Show();
    Cursor::Arrow();
    win->Close(0);
    delete win;
    win = pinWin;
}

void App::Quit(const int& code) {
    exitCode = code;
    win->Close(0);
    delete win;
    PostQuitMessage(0);
}

void App::initFontIcon()
{
    HMODULE instance = GetModuleHandle(NULL);
    HRSRC resID = FindResource(instance, MAKEINTRESOURCE(IDR_ICON_FONT), L"ICON_FONT");
    if (resID == 0)
    {
        MessageBox(NULL, L"查找字体图标资源失败", L"系统提示", NULL);
        return;
    }
    size_t resSize = SizeofResource(instance, resID);
    HGLOBAL res = LoadResource(instance, resID);
    if (res == 0)
    {
        MessageBox(NULL, L"加载字体图标资源失败", L"系统提示", NULL);
        return;
    }
    LPVOID resData = LockResource(res);
    auto fontData = SkData::MakeWithoutCopy(resData, resSize);
    auto iconFace = SkTypeface::MakeFromData(fontData);
    fontIcon = new SkFont(iconFace);
}

void App::initFontText()
{
	auto fontFace = SkTypeface::MakeFromName("Microsoft YaHei", SkFontStyle::Normal());
	fontText = new SkFont(fontFace);
}
void App::initScreens() {
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &info);
            POINT leftTop{ .x{info.rcMonitor.left},.y{info.rcMonitor.top} };
            POINT rightBottom{ .x{info.rcMonitor.right},.y{info.rcMonitor.bottom} };
            auto hwnd = App::GetWin()->hwnd;
            ScreenToClient(hwnd, &leftTop);
            ScreenToClient(hwnd, &rightBottom);
            auto ptr = std::make_shared<SkRect>(SkRect::MakeLTRB(leftTop.x, leftTop.y, rightBottom.x + 1, rightBottom.y + 1));
            screens.push_back(ptr);
            return TRUE;
        }, NULL);
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