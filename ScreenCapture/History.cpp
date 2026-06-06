#include "pch.h"
#include "History.h"
#include "Win/WinPin.h"
#include "Win/WinToolMain.h"
#include "Win/WinToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeEraser.h"
#include "Shape/ShapeTextWin.h"

History::History(WinPin* win):win{win}
{

}

History::~History()
{

}
ShapeBase* History::createShape(const std::string& state, const int& x, const int& y)
{
    removeUndoShape();
    ShapeBase* result{nullptr};
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "rect") {
        auto shape = std::make_unique<ShapeRect>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "ellipse") {
        auto shape = std::make_unique<ShapeEllipse>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "arrow") {
        auto shape = std::make_unique<ShapeArrow>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "number") {
        auto shape = std::make_unique<ShapeNumber>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "line") {
        auto shape = std::make_unique<ShapeLine>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "text") {
        auto shape = std::make_unique<ShapeText>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "mosaic") {
        auto shape = std::make_unique<ShapeMosaic>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->state == "eraser") {
        auto shape = std::make_unique<ShapeEraser>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    result->mouseDown((float)x, (float)y);
    return result;
}

void History::undo()
{
    int i{ (int)(shapes.size() - 1) };
    for (; i >= 0; i--)
    {
        auto cur = shapes[i].get();
        if (!cur->isUndo) {
            cur->isUndo = true;
            if (cur == win->shapeHover) {
                win->shapeHover = nullptr;
            }
            win->refresh();
            break;
        }
    }
}

void History::redo()
{
    for (size_t i = 0; i < shapes.size(); i++)
    {
        auto cur = shapes[i].get();
        if (cur->isUndo) {
            cur->isUndo = false;
            win->refresh();
            break;
        }
    }
}

/// <summary>
/// 进删除除 hover 状态的 shape
/// </summary>
void History::removeHoverShape()
{
    if (!win->shapeHover) return;
    auto target = win->shapeHover;
    // 如果正在编辑文本，先隐藏编辑窗口
    if (auto txt = dynamic_cast<ShapeText*>(target)) {
        if (txt->isEditing) {
            if (auto tw = ShapeTextWin::get()) tw->hide();
            txt->isEditing = false;
        }
    }
    // 从历史中移除目标 shape
    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (it->get() == target) {
            shapes.erase(it);
            break;
        }
    }
    win->shapeHover = nullptr;
    win->refresh();
}

void History::removeUndoShape()
{
    int i{ (int)(shapes.size() - 1) };
    for (; i >= 0; i--)
    {
        auto cur = shapes[i].get();
        if (!cur->isUndo) {
            break;
        }
        shapes.erase(shapes.begin() + i);
    }
}
