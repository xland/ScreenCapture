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
    HoverShape = nullptr;
    if (CurShape) {        
        if (typeid(*CurShape) == typeid(ShapeText)) {
            if (CurShape->IsWip) { //正在写字
                bool flag = CurShape->OnMouseDown(x, y);
                if (flag) { //不在输入框内，结束写字
                    auto textObj = static_cast<ShapeText*>(CurShape);
                    if (textObj->EndInput()) { //还没输入，则删除此对象
                        std::erase_if(shapes, [this](auto& item) { return item.get() == CurShape; });
                        App::GetWin()->ClearTimeout(WM_FLASH_CURSOR);
                    }
                    CurShape = nullptr;
                }
                return true;
            }
            else { //没在写字
                bool flag = CurShape->OnMouseDown(x, y);
                if (flag) { //不在输入框内
                    return false;
                }
            }
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
            else {
                if (shape->IsDel) {
                    break;
                }
                else {
                    shape->Paint(canvasBack);
                }                
            }
        }
        win->Refresh();        
    }
    else {
        createShape(x, y, win->state);
        CurShape = shapes.back().get();
        CurShape->OnMouseDown(x, y);        
    }
    return false;
}
bool Recorder::OnMouseDownRight(const int& x, const int& y)
{
    //正在写字，右键点击 结束写字
    if (CurShape && typeid(*CurShape) == typeid(ShapeText) ) {
        auto textObj = static_cast<ShapeText*>(CurShape);
        auto flag = textObj->EndInput();
        if (flag) {
            std::erase_if(shapes, [this](auto& item) { return item.get() == CurShape; });
        }
        CurShape = nullptr;
        return true;
    }
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    HoverShape = nullptr;
    if (!CurShape)
    {
        return false;
    }
    if (CurShape->IsWip && typeid(*CurShape) == typeid(ShapeText)) {
        return false;
    }
    if (CurShape->IsTemp)
    {
        std::erase_if(shapes, [this](auto& item) { return item.get() == CurShape; });
        CurShape = nullptr;
        return false;
    }
    CurShape->OnMouseUp(x, y);
    auto win = App::GetWin();
    auto canvasBack = win->surfaceBack->getCanvas();
    canvasBack->clear(SK_ColorTRANSPARENT);
    auto canvasFront = win->surfaceFront->getCanvas();
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
    CurShape = nullptr;
    return false;
}
bool Recorder::OnMouseMove(const int &x, const int &y)
{
    auto win = App::GetWin();
    if (win->state <= State::tool) {
        return false;
    }
    if (win->state == State::text) {
        Cursor::Text();
    }
    else {
        Cursor::Cross();
    }
    if (shapes.size() == 0) {
        return false;
    }
    //写字时，鼠标移动到其他元素上必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
    if (CurShape && CurShape->IsWip && typeid(*CurShape) == typeid(ShapeText)) {
        return false;
    }
    bool isHover = false;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->IsDel) {
            break;
        }
        isHover = shapes[i]->OnMouseMove(x, y);
        if (isHover)
        {            
            if (HoverShape != shapes[i].get()) {
                HoverShape = shapes[i].get();
                win->ClearTimeout(WM_SHOW_DRAGGER);
                win->SetTimeout(WM_SHOW_DRAGGER, 1600);
            }
            break;
        }
    }
    if (!isHover) {
        HoverShape = nullptr;
        win->ClearTimeout(WM_SHOW_DRAGGER);
    }
    return false;
}
bool Recorder::OnMouseDrag(const int &x, const int &y)
{
    HoverShape = nullptr;
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
            auto textObj = static_cast<ShapeText*>(CurShape);
            auto flag = textObj->EndInput();
            if (flag) {
                std::erase_if(shapes, [this](auto& item) { return item.get() == CurShape; }); 
                App::GetWin()->ClearTimeout(WM_FLASH_CURSOR);
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
bool Recorder::OnTimeout(const unsigned int& id)
{
    switch (id)
    {
    case WM_SHOW_DRAGGER: {
        auto win = App::GetWin();
        win->ClearTimeout(WM_SHOW_DRAGGER);
        if (HoverShape) {
            HoverShape->ShowDragger();
            CurShape = HoverShape;
            if (typeid(*CurShape) != typeid(ShapeText)) {
                win->SetTimeout(WM_HIDE_DRAGGER, 1600);
            }            
        }
        break;
    }
    case WM_HIDE_DRAGGER: {
        auto win = App::GetWin();
        if (!CurShape) {
            win->ClearTimeout(WM_HIDE_DRAGGER);
        }
        else if (HoverShape != CurShape && !win->IsMouseDown) {
            win->ClearTimeout(WM_HIDE_DRAGGER);
            CurShape->HideDragger();
            CurShape = nullptr;
        }
        break;
    }
    case WM_FLASH_CURSOR: {
        if (CurShape) {
            CurShape->Paint(nullptr);
        }
        break;
    }
    default:
        break;
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
    if (!flag && CurShape) {
        CurShape->HideDragger();
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
    std::erase_if(shapes, [](auto& item) {return item->IsDel; });
    shapes.push_back(std::move(shape));    
}