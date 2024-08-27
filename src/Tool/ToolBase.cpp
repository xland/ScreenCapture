#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ToolBase.h"
#include "../WinMax.h"
#include "../App.h"

ToolBase::ToolBase() :IndexHovered{ -1 }, IndexSelected{ -1 }
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
        if (IndexHovered >= 0)
        {
            Btns[IndexHovered]->IsHover = false;
            IndexHovered = -1;
            win->Refresh();
        }
        return;
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
