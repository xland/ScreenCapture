#include <qpainter.h>

#include "ShapeRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"

ShapeRect::ShapeRect(QObject* parent) : ShapeRectBase(parent)
{
    auto win = (WinBase*)parent;
    isFill = win->toolSub->getSelectState("rectFill");
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeRect::~ShapeRect()
{
}
