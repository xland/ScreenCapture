#include "Screen.h"
#include "Windows.h"
#include "App.h"
#include "WindowBase.h"

static std::vector<std::shared_ptr<SkRect>> screens;

std::shared_ptr<SkRect> Screen::GetScreen(const float& x, const float& y)
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
            POINT leftTop{ .x{info.rcMonitor.left},.y{info.rcMonitor.top} };
            POINT rightBottom{ .x{info.rcMonitor.right},.y{info.rcMonitor.bottom} };
            auto hwnd = App::GetWin()->hwnd;
            ScreenToClient(hwnd, &leftTop);
            ScreenToClient(hwnd, &rightBottom);
            auto ptr = std::make_shared<SkRect>(SkRect::MakeLTRB(leftTop.x, leftTop.y, rightBottom.x+1, rightBottom.y+1));
            screens.push_back(ptr);
            return TRUE;
        }, NULL);
}
