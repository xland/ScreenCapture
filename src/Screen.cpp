#include "Screen.h"
#include <Windows.h>
#include "include/core/SkRect.h"
#include <memory>
#include <vector>

namespace {
    std::vector<RECT> screens;
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::Init()
{
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam)
        {
            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &info);
            screens.push_back(info.rcMonitor);
            return TRUE;
        }, NULL);
}
