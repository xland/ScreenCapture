#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ToolBase.h"
#include "../WinMax.h"
#include "../App.h"

ToolBase::ToolBase() :indexHovered{ -1 }, indexSelected{ -1 }
{
}

ToolBase::~ToolBase()
{
}

void ToolBase::OnMouseMove(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state < State::tool)
    {
        return;
    }
    if (!ToolRect.contains(x, y))
    {
        if (indexHovered >= 0)
        {
            Btns[indexHovered].isHover = false;
            indexHovered = -1;
            win->Refresh();
        }
        return;
    }
    SetCursor(LoadCursor(nullptr, IDC_HAND));
    int index = (x - ToolRect.left()) / ToolBtn::Width;
    if (index != indexHovered)
    {
        Btns[index].isHover = true;
        if (indexHovered >= 0) {
            Btns[indexHovered].isHover = false;
        }
        indexHovered = index;
        win->Refresh();
    }
    return;
}

void ToolBase::OnLeftBtnUp(const int& x, const int& y)
{
    isMouseDown = false;
    if (ToolRect.contains(x, y)) {
        return;
    }
    return;
}

void ToolBase::OnMouseDrag(const int& x, const int& y)
{
    if (isMouseDown) {
        return;
    }
    return;
}
