#pragma once
#include <Windows.h>
#include "include/core/SkFont.h"
class WindowBase;
class App
{
public:
    ~App();
    static void Init(std::wstring&& cmd);
    static void Dispose();
    static void Quit(const int& code);
    static WindowBase* GetWin();
    static SkFont* GetFontIcon();
    static SkFont* GetFontText();
    static void Pin();
    static int GetExitCode();
    static std::shared_ptr<SkRect> GetScreen(const float& x, const float& y);
    static void Log(std::string&& info);
    static void SaveFile();
private:
    App() {};
    static void initFontIcon();
    static void initFontText();
    static void initScreens();
    static void createWindow();
    static void pinClipboard();
    static std::wstring createFileName();
    static std::string toStdString(std::wstring&& wstr);
};