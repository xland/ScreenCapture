#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include <vector>
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>

class ToolBtn
{
public:
    ToolBtn(const char *icon, std::wstring &&tip, bool isDisable = false, bool selectable = true, int fontSize = 22, SkColor fontColor = SK_ColorTRANSPARENT, bool isSelected = false);
    ~ToolBtn();
    void Paint(SkCanvas *canvas, SkPaint &paint, float &x, float &y);    
    const static int width{50};
    const static int height{46};
    const char *icon;
    int fontSize;
    SkColor fontColor;
    bool isSelected;
    bool isHover;
    bool isDisable;
    bool selectable;
    TOOLINFO tipInfo{0};
protected:
private:
    inline void setToolTip(const int& x, const int& y);
};