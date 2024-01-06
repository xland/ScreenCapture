#pragma once
#include "include/core/SkFont.h"
class WindowBase;
class App
{
public:
    ~App();
    static void Init();
    static void Dispose();
    static WindowBase* GetWin();
    static SkFont* GetFontIcon();
    static SkFont* GetFontText();

private:
    App() {};
    static void initFontIcon();
    static void initFontText();
};