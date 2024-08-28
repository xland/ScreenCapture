#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "include/core/SkCanvas.h"
#include "../EventHandler.h"

class ToolBtn:public EventHandler
{
public:
    ToolBtn(const char* icon, std::wstring&& tip, bool isDisable = false, bool selectable = true, int fontSize = 22, SkColor fontColor = SK_ColorTRANSPARENT, bool isSelected = false);
    ToolBtn(const int& id);
    ~ToolBtn();
    void Paint(SkCanvas* canvas, SkPaint& paint, float& x, float& y);
    void OnLeftBtnDown(const int& x, const int& y);
    void OnLeftBtnUp(const int& x, const int& y);
    void OnMouseMove(const int& x, const int& y);
    const static int Width{ 50 };
    const static int Height{ 46 };
    const char* iconCode;
    SkColor fontColor{ SK_ColorTRANSPARENT };
    bool isSelected{ false };
    bool isHover{ false };
    bool isDisable{ false };
    bool selectable{ true };
    int x;
    int y;
    int id;
protected:
private:
    void initIconCode(const int& id);
    inline void setToolTip(const int& x, const int& y);
    int fontSize{22};
    TOOLINFO tipInfo{ 0 };
};