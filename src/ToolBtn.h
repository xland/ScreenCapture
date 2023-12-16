#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include <vector>

class ToolBtn
{
public:
    ToolBtn(const char* icon,std::wstring&& tip,bool isDisable=false,bool selectable=true,int fontSize = 22, SkColor fontColor=SK_ColorTRANSPARENT);
    ~ToolBtn();
    void Paint(SkCanvas* canvas, SkPaint& paint, float& x, float& y);
    const static int width{50};
    const static int height{46};
    const char* icon;
    int fontSize;
    SkColor fontColor;
    std::wstring tip;
    bool isSelected;
    bool isHover;
    bool isDisable;
    bool selectable;
protected:
private:

};