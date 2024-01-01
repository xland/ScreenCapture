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
    if (!curShape)
    {
        createShape(x, y, win->state);
        auto curIndex = shapes.size() - 1;
        curShape = shapes[curIndex].get();
        curShape->OnMouseDown(x, y);
    }
    else {
        curShape->isWip = true;
        curShape->OnMouseDown(x, y);
        auto canvasBack = win->surfaceBack->getCanvas();
        auto canvasFront = win->surfaceFront->getCanvas();
        canvasBack->clear(SK_ColorTRANSPARENT);
        for (auto& shape : shapes)
        {
            if (shape->isWip) {
                shape->Paint(canvasFront);                
            }
            else {
                shape->Paint(canvasBack);
            }
        }
        win->Refresh();
    }
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    if (!curShape)
    {
        return false;
    }
    auto win = WindowMain::get();
    if (win->state == State::text) {
        return false;
    }
    // todo 
    //if (shapes[curIndex]->isWip)
    //{
    //    shapes.erase(shapes.begin() + curIndex, shapes.begin() + 1);
    //}
    // 
    curShape->OnMouseUp(x, y);
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    bool undoDisable = true;
    bool redoDisable = true;
    for (auto& shape : shapes)
    {
        if (shape->isWip) {
            redoDisable = false;
        }
        else {
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
    auto isMouseOnShape = false;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->OnMouseMove(x, y);
        if (flag)
        {
            curShape = shapes[i].get();
            isMouseOnShape = true;
            break;
        }
    }
    if (!isMouseOnShape) {
        curShape = nullptr;
    }
    auto shapeDragger = ShapeDragger::get();
    int index = shapeDragger->indexMouseAt(x, y);
    if (index >= 0) {
        curShape = shapeDragger->curShape;
    }
    if (curShape)
    {
        if (typeid(*curShape) == typeid(ShapeText)) {
            return false;
        }
        auto canvas = win->surfaceFront->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        shapeDragger->showDragger(canvas);
        win->Refresh();
    }
    else {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
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
        if (!shapes[i]->isWip)
        {
            shapes[i]->isWip = true;
            redoDisable = false;
            break;
        }
    }
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->isWip)
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
        if (shapes[i]->isWip)
        {
            shapes[i]->isWip = false;
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
    switch (state)
    {
    case State::rect:
    {
        shapes.push_back(std::make_shared<ShapeRect>(x, y));
        break;
    }
    case State::ellipse:
    {
        shapes.push_back(std::make_shared<ShapeEllipse>(x, y));
        break;
    }
    case State::arrow:
    {
        shapes.push_back(std::make_shared<ShapeArrow>(x, y));
        break;
    }
    case State::number:
    {
        shapes.push_back(std::make_shared<ShapeNumber>(x, y));
        break;
    }
    case State::pen:
    {
        shapes.push_back(std::make_shared<ShapePen>(x, y));
        break;
    }
    case State::line:
    {
        shapes.push_back(std::make_shared<ShapeLine>(x, y));
        break;
    }
    case State::text:
    {
        shapes.push_back(std::make_shared<ShapeText>(x, y));        
        break;
    }
    case State::mosaic:
    {
        shapes.push_back(std::make_shared<ShapeMosaic>(x, y));
        break;
    }
    case State::eraser:
    {
        shapes.push_back(std::make_shared<ShapeEraser>(x, y));
        break;
    }
    default:
        break;
    }
}