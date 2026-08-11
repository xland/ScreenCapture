#include "pch.h"
#include "History.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeEraser.h"

History::History(WinPin* win):win{win}
{

}

History::~History()
{

}
ShapeBase* History::createShape(const std::wstring& state, const int& x, const int& y)
{
    removeUndoShape();
    ShapeBase* result{nullptr};
    auto toolMain = win->toolMain.get();
    if (toolMain->curId == L"rect") {
        auto shape = std::make_unique<ShapeRect>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"ellipse") {
        auto shape = std::make_unique<ShapeEllipse>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"arrow") {
        auto shape = std::make_unique<ShapeArrow>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"number") {
        auto shape = std::make_unique<ShapeNumber>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"line") {
        auto shape = std::make_unique<ShapeLine>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"text") {
        auto shape = std::make_unique<ShapeText>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"mosaic") {
        auto shape = std::make_unique<ShapeMosaic>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    else if (toolMain->curId == L"eraser") {
        auto shape = std::make_unique<ShapeEraser>(win);
        result = shape.get();
        shapes.push_back(std::move(shape));
    }
    // curId 落在没有对应 shape 的工具上时 result 是空的，直接返回
    if (!result) return nullptr;
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
    // 正在编辑的话先收尾：TextBox 是 WinPin 上共用的一个，
    // 删了 shape 却留着它显示，下一次编辑就会带着上一次的文字。
    if (auto txt = dynamic_cast<ShapeText*>(target)) {
        txt->finishEdit();
    }
    removeShape(target);
}

void History::removeShape(ShapeBase* target)
{
    for (auto it = shapes.begin(); it != shapes.end(); ++it) {
        if (it->get() == target) {
            shapes.erase(it);
            break;
        }
    }
    // hover 指针指向的正是刚被销毁的那个元素时必须清掉，否则下一次绘制/命中就是野指针
    if (win->shapeHover == target) {
        win->shapeHover = nullptr;
    }
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
