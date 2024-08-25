#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "include/core/SkCanvas.h"

class ToolBtn
{
public:
    ToolBtn(const char* icon, std::wstring&& tip, bool isDisable = false, bool selectable = true, int fontSize = 22, SkColor fontColor = SK_ColorTRANSPARENT, bool isSelected = false);
    ~ToolBtn();
    void Paint(SkCanvas* canvas, SkPaint& paint, float& x, float& y);
    const static int Width{ 50 };
    const static int Height{ 46 };
    const char* Icon;
    SkColor FontColor;
    bool IsSelected;
    bool IsHover;
    bool IsDisable;
    bool Selectable;
protected:
private:
    inline void setToolTip(const int& x, const int& y);
    int fontSize;
    TOOLINFO tipInfo{ 0 };
};