#include "Recorder.h"
#include "App.h"
#include "WindowBase.h"
#include "Cursor.h"
#include "ToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapePen.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeEraser.h"
#include "Shape/ShapeEraserRect.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeMosaicRect.h"
#include "Shape/ShapeDragger.h"
#include "ToolMain.h"

Recorder *recorder;

Recorder::Recorder()
{
}
Recorder::~Recorder()
{
}
void Recorder::Init()
{
    recorder = new Recorder();
}
Recorder *Recorder::Get()
{
    return recorder;
}
bool Recorder::OnMouseDown(const int &x, const int &y)
{
    auto win = App::GetWin();
    if (win->state < State::rect)
    {
        return false;
    }
    if (CurShape)
    {        
        if (CurShape && CurShape->IsWip && typeid(*CurShape) == typeid(ShapeText) ) { //正在写字，左键单击无效
            CurShape->OnMouseDown(x, y);
            return false;
        }
        CurShape->IsWip = true;
        CurShape->OnMouseDown(x, y);
        auto canvasBack = win->surfaceBack->getCanvas();
        auto canvasFront = win->surfaceFront->getCanvas();
        canvasBack->clear(SK_ColorTRANSPARENT);
        canvasFront->clear(SK_ColorTRANSPARENT);
        for (auto& shape : shapes)
        {
            if (shape->IsWip) {
                shape->Paint(canvasFront);
            }
            else if (!shape->IsDel) {
                shape->Paint(canvasBack);
            }
        }
        win->Refresh();        
    }
    else {
        createShape(x, y, win->state);
        auto curIndex = shapes.size() - 1;
        CurShape = shapes[curIndex].get();
        CurShape->OnMouseDown(x, y);
    }
    return false;
}
bool Recorder::OnMouseDownRight(const int& x, const int& y)
{
    //正在写字，右键点击 结束写字
    if (CurShape && typeid(*CurShape) == typeid(ShapeText) ) {
        auto textObj = dynamic_cast<ShapeText*>(CurShape);
        auto flag = textObj->EndInput();
        if (flag) {
            auto iter = std::remove_if(shapes.begin(), shapes.end(), [this](auto item) { return item.get() == CurShape; });
            shapes.erase(iter, shapes.end());
        }
        return true;
    }
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    if (!CurShape)
    {
        return false;
    }
    if (CurShape && CurShape->IsWip && typeid(*CurShape) == typeid(ShapeText)) {
        return false;
    }
    if (CurShape->IsTemp)
    {
        auto iter = std::remove_if(shapes.begin(), shapes.end(), [this](auto item) { return item.get() == CurShape; });
        shapes.erase(iter, shapes.end());
        CurShape = nullptr;
        return false;
    }
    CurShape->OnMouseUp(x, y);
    auto win = App::GetWin();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    bool undoDisable = true;
    bool redoDisable = true;
    //必须全部重绘一次，不然修改历史元素时，无法保证元素的绘制顺序
    for (auto& shape : shapes)
    {
        if (shape->IsDel) {
            redoDisable = false;
        }
        else{
            shape->Paint(canvasBack); 
            undoDisable = false;
        }
    }
    auto toolMain = ToolMain::Get();
    toolMain->SetUndoDisable(undoDisable);
    toolMain->SetRedoDisable(redoDisable);
    ShapeDragger::Get()->ShowDragger(canvasFront);
    win->Refresh();
    return false;
}
bool Recorder::OnMouseMove(const int &x, const int &y)
{
    auto win = App::GetWin();
    if (win->state == State::text) {
        Cursor::Text();
    }
    else {
        Cursor::Cross();
    }
    if (win->state <= State::tool || shapes.size() == 0) {
        return false;
    }
    //写字时，鼠标移动到其他元素上必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
    if (CurShape && CurShape->IsWip && typeid(*CurShape) == typeid(ShapeText)) {
        CurShape->OnMouseMove(x, y);
        return false;
    }
    CurShape = nullptr;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->IsDel) continue;
        auto flag = shapes[i]->OnMouseMove(x, y);
        if (flag)
        {
            CurShape = shapes[i].get();
            break;
        }
    }
    auto shapeDragger = ShapeDragger::Get();
    int index = shapeDragger->IndexMouseAt(x, y);
    if (index >= 0) {
        CurShape = shapeDragger->CurShape;
    }
    if (CurShape)
    {
        auto canvas = win->surfaceFront->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        shapeDragger->ShowDragger(canvas);
        win->Refresh();
    }
    else {       
        Timer::Get()->Start(0, 800, []() {
            return ShapeDragger::Get()->HideDragger();
            });
    }
    return false;
}
bool Recorder::OnMouseDrag(const int &x, const int &y)
{
    if (CurShape)
    {
        CurShape->OnMoseDrag(x, y);
    }
    return false;
}
bool Recorder::OnChar(const unsigned int& val)
{
    if (CurShape)
    {
        CurShape->OnChar(val);
    }
    return false;    
}
bool Recorder::OnKeyDown(const unsigned int& val)
{
    if (CurShape)
    {
        if (val == VK_ESCAPE && typeid(*CurShape) == typeid(ShapeText)) {
            auto textObj = dynamic_cast<ShapeText*>(CurShape);
            auto flag = textObj->EndInput();
            if (flag) {
                auto iter = std::remove_if(shapes.begin(), shapes.end(), [this](auto item) { return item.get() == CurShape; });
                shapes.erase(iter, shapes.end());
            }
            CurShape = nullptr;
            return true;
        }
        else
        {
            return CurShape->OnKeyDown(val);
        }
    }
    return false;
}
bool Recorder::OnMouseWheel(const int& delta)
{
    if (CurShape)
    {
        CurShape->OnMouseWheel(delta);
    }
    return false;
}
void Recorder::Undo()
{
    bool undoDisable = true;
    bool redoDisable = true;
    auto win = App::GetWin();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (!shapes[i]->IsDel)
        {
            shapes[i]->IsDel = true;
            redoDisable = false;
            break;
        }
    }
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->IsDel)
        {
            break;
        }
        else {
            shapes[i]->Paint(canvasBack);
            undoDisable = false;
        }
    }
    auto toolMain = ToolMain::Get();
    toolMain->SetUndoDisable(undoDisable);
    toolMain->SetRedoDisable(redoDisable);
    win->Refresh();
}
void Recorder::Redo()
{
    bool undoDisable = true;
    bool redoDisable = true;
    auto win = App::GetWin();
    auto canvasBack = win->surfaceBack->getCanvas();
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    canvasFront->clear(SK_ColorTRANSPARENT);
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->IsDel)
        {
            shapes[i]->IsDel = false;
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
    auto toolMain = ToolMain::Get();
    toolMain->SetUndoDisable(undoDisable);
    toolMain->SetRedoDisable(redoDisable);
    win->Refresh();
}
void Recorder::FinishPaint()
{
    auto flag = OnMouseDownRight(-1, -1);
    if (!flag) {
        ShapeDragger::Get()->HideDragger();
    } 
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
        shape = std::make_shared<ShapeNumber>(x-36, y-36);
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
        auto tool = ToolSub::Get();
        if (tool->GetFill()) {
            shape = std::make_shared<ShapeMosaicRect>(x, y);
        }
        else {
            shape = std::make_shared<ShapeMosaic>(x, y);
        }        
        break;
    }
    case State::eraser:
    {
        auto tool = ToolSub::Get();
        if (tool->GetFill()) {
            shape = std::make_shared<ShapeEraserRect>(x, y);
        }
        else {
            shape = std::make_shared<ShapeEraser>(x, y);
        }
        break;
    }
    default:
        break;
    }
    int index = shapes.size();
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->IsDel) {
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