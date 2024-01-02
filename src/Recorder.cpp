#include "Recorder.h"
#include "WindowMain.h"
#include "Icon.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapePen.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeEraser.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeDragger.h"
#include "ToolMain.h"

Recorder *recorder;

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
}

void Recorder::init()
{
    if (!recorder)
    {
        recorder = new Recorder();
    }
}

Recorder *Recorder::get()
{
    return recorder;
}

bool Recorder::OnMouseDown(const int &x, const int &y)
{
    auto win = WindowMain::get();
    if (win->state < State::rect)
    {
        return false;
    }
    if (curShape)
    {
        //正在写字，左键单击无效
        if (curShape && typeid(*curShape) == typeid(ShapeText) ) {
            if (curShape->isWip) {
                curShape->OnMouseDown(x, y);
                return false;
            }
            else
            {
                auto temp = dynamic_cast<ShapeText*>(curShape);
                temp->showCursor = true;
            }
        }
        curShape->isWip = true;
        curShape->OnMouseDown(x, y);
        auto canvasBack = win->surfaceBack->getCanvas();
        auto canvasFront = win->surfaceFront->getCanvas();
        canvasBack->clear(SK_ColorTRANSPARENT);
        canvasFront->clear(SK_ColorTRANSPARENT);
        for (auto& shape : shapes)
        {
            if (shape->isWip) {
                shape->Paint(canvasFront);
            }
            else if (!shape->isDel) {
                shape->Paint(canvasBack);
            }
        }
        win->Refresh();        
    }
    else {
        createShape(x, y, win->state);
        auto curIndex = shapes.size() - 1;
        curShape = shapes[curIndex].get();
        curShape->OnMouseDown(x, y);
    }
    return false;
}
bool Recorder::OnMouseDownRight(const int& x, const int& y)
{
    //正在写字，右键点击 结束写字
    if (curShape && curShape->isWip && typeid(*curShape) == typeid(ShapeText) ) {
        curShape->isWip = false;
        Timer::get()->Remove(1);
        auto win = WindowMain::get();
        auto canvasFront = win->surfaceFront->getCanvas();
        canvasFront->clear(SK_ColorTRANSPARENT);
        auto canvasBack = win->surfaceBack->getCanvas();
        curShape->Paint(canvasBack);
        win->Refresh();
        curShape = nullptr;
        auto toolMain = ToolMain::get();
        toolMain->setUndoDisable(false);
    }
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    if (!curShape)
    {
        return false;
    }
    if (curShape && curShape->isWip && typeid(*curShape) == typeid(ShapeText)) {
        return false;
    }
    // todo 
    //if (shapes[curIndex]->isWip)
    //{
    //    shapes.erase(shapes.begin() + curIndex, shapes.begin() + 1);
    //}
    // 
    curShape->OnMouseUp(x, y);

    auto win = WindowMain::get();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    bool undoDisable = true;
    bool redoDisable = true;
    //必须全部重绘一次，不然修改历史元素时，无法保证元素的绘制顺序
    for (auto& shape : shapes)
    {
        if (shape->isDel) {
            redoDisable = false;
        }
        else{
            shape->Paint(canvasBack); 
            undoDisable = false;
        }
    }
    auto toolMain = ToolMain::get();
    toolMain->setUndoDisable(undoDisable);
    toolMain->setRedoDisable(redoDisable);
    ShapeDragger::get()->showDragger(canvasFront);
    win->Refresh();
    return false;
}
bool Recorder::OnMouseMove(const int &x, const int &y)
{
    auto win = WindowMain::get();
    if (win->state <= State::tool || shapes.size() == 0) {
        return false;
    }
    //写字时，鼠标移动到其他元素上必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
    if (curShape && curShape->isWip && typeid(*curShape) == typeid(ShapeText)) {
        curShape->OnMouseMove(x, y);
        return false;
    }
    curShape = nullptr;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->OnMouseMove(x, y);
        if (flag)
        {
            curShape = shapes[i].get();
            break;
        }
    }
    auto shapeDragger = ShapeDragger::get();
    int index = shapeDragger->indexMouseAt(x, y);
    if (index >= 0) {
        curShape = shapeDragger->curShape;
    }
    if (curShape)
    {
        auto canvas = win->surfaceFront->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        shapeDragger->showDragger(canvas);
        win->Refresh();
    }
    else {
        Icon::myCursor(Icon::cursor::arrow);
        Timer::get()->Start(0, 800, []() {
            return ShapeDragger::get()->hideDragger();
            });
    }
    return false;
}
bool Recorder::OnMouseDrag(const int &x, const int &y)
{
    if (curShape)
    {
        curShape->OnMoseDrag(x, y);
    }
    return false;
}
bool Recorder::onChar(const unsigned int& val)
{
    if (curShape)
    {
        curShape->OnChar(val);
    }
    return false;    
}
bool Recorder::onKeyDown(const unsigned int& val)
{
    if (curShape)
    {
        curShape->OnKeyDown(val);
    }
    return false;
}

bool Recorder::onMouseWheel(const int& delta)
{
    if (curShape)
    {
        curShape->onMouseWheel(delta);
    }
    return false;
}

void Recorder::undo()
{
    bool undoDisable = true;
    bool redoDisable = true;
    auto win = WindowMain::get();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (!shapes[i]->isDel)
        {
            shapes[i]->isDel = true;
            redoDisable = false;
            break;
        }
    }
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->isDel)
        {
            break;
        }
        else {
            shapes[i]->Paint(canvasBack);
            undoDisable = false;
        }
    }
    auto toolMain = ToolMain::get();
    toolMain->setUndoDisable(undoDisable);
    toolMain->setRedoDisable(redoDisable);
    win->Refresh();
}

void Recorder::redo()
{
    bool undoDisable = true;
    bool redoDisable = true;
    auto win = WindowMain::get();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->isDel)
        {
            shapes[i]->isDel = false;
            shapes[i]->Paint(canvasBack);
            undoDisable = false;
            if (i < shapes.size() - 1) {
                redoDisable = false;
            }
            break;
        }
        else {
            shapes[i]->Paint(canvasBack);
            undoDisable = false;
        }
    }
    auto toolMain = ToolMain::get();
    toolMain->setUndoDisable(undoDisable);
    toolMain->setRedoDisable(redoDisable);
    win->Refresh();
}

void Recorder::createShape(const int &x, const int &y, const State &state)
{
    std::shared_ptr<ShapeBase> shape;
    switch (state)
    {
    case State::rect:
    {
        shape = std::make_shared<ShapeRect>(x, y);
        break;
    }
    case State::ellipse:
    {
        shape = std::make_shared<ShapeEllipse>(x, y);
        break;
    }
    case State::arrow:
    {
        shape = std::make_shared<ShapeArrow>(x, y);
        break;
    }
    case State::number:
    {
        shape = std::make_shared<ShapeNumber>(x, y);
        break;
    }
    case State::pen:
    {
        shape = std::make_shared<ShapePen>(x, y);
        break;
    }
    case State::line:
    {
        shape = std::make_shared<ShapeLine>(x, y);
        break;
    }
    case State::text:
    {
        shape = std::make_shared<ShapeText>(x, y);
        break;
    }
    case State::mosaic:
    {
        shape = std::make_shared<ShapeMosaic>(x, y);
        break;
    }
    case State::eraser:
    {
        shape = std::make_shared<ShapeEraser>(x, y);
        break;
    }
    default:
        break;
    }
    int index = shapes.size();
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->isDel) {
            index = i - 1;
            break;
        }
    }
    if (index < 0) index = 0;
    if (index < shapes.size()) {
        shapes.erase(shapes.begin() + index,shapes.end());
    }
    shapes.push_back(std::move(shape));
}