#pragma once
#include <Windows.h>
#include <string>
#include <memory>

class WinBase;
class App
{
public:
    App();
    ~App();
    static void Init(HINSTANCE instance, std::wstring&& cmd);
    static App* Get();
    static WinBase* GetWin();
    static int GetExitCode();
    HINSTANCE instance;
private:
    std::unique_ptr<WinBase> win;
};