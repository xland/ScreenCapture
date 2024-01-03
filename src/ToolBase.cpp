#include "ToolBase.h"
#include "Icon.h"
#include "WindowMain.h"

ToolBase::ToolBase() :IndexHovered{ -1 }, IndexSelected{ -1 }
{

}

ToolBase::~ToolBase()
{
}

bool ToolBase::OnMouseMove(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state < State::tool)
    {
        return false;
    }
    if (!ToolRect.contains(x, y))
    {
        if (IndexHovered >= 0)
        {
            btns[IndexHovered]->isHover = false;
            IndexHovered = -1;
            winMain->Refresh();
        }
        return false;
    }
    Icon::myCursor(Icon::cursor::hand);
    int index = (x - ToolRect.left()) / ToolBtn::width;
    if (index != IndexHovered)
    {
        btns[index]->isHover = true;
        if (IndexHovered >= 0) {
            btns[IndexHovered]->isHover = false;
        }
        IndexHovered = index;
        winMain->Refresh();
    }
    return true;
}
