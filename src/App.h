#pragma once
#include <Windows.h>
#include "include/core/SkFont.h"
class WindowBase;
class App
{
public:
    ~App();
    static void Init(HINSTANCE instance, std::wstring&& cmd);
    static App* Get();
    static int GetExitCode();
    HINSTANCE instance;
private:
    App() {};
};