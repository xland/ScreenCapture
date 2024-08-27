#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ToolBase.h"
#include "../WinMax.h"

ToolBase::ToolBase() :IndexHovered{ -1 }, IndexSelected{ -1 }
{
}

ToolBase::~ToolBase()
{
}

bool ToolBase::onMouseMove(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        if (IndexHovered >= 0)
        {
            Btns[IndexHovered]->IsHover = false;
            IndexHovered = -1;
            win->Refresh();
        }
        return false;
    }
    SetCursor(LoadCursor(nullptr, IDC_HAND));
    int index = (x - ToolRect.left()) / ToolBtn::Width;
    if (index != IndexHovered)
    {
        Btns[index]->IsHover = true;
        if (IndexHovered >= 0) {
            Btns[IndexHovered]->IsHover = false;
        }
        IndexHovered = index;
        win->Refresh();
    }
    return true;
}

bool ToolBase::onLeftBtnUp(const int& x, const int& y)
{
    isMouseDown = false;
    if (ToolRect.contains(x, y)) {
        return true;
    }
    return false;
}

bool ToolBase::onMouseDrag(const int& x, const int& y)
{
    if (isMouseDown) {
        return true;
    }
    return false;
}
