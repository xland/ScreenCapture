#include "Recorder.h"
#include "Shape/ShapeBase.h"
#include "WindowMain.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapePen.h"
#include "Shape/ShapeLine.h"
Recorder *recorder;

Recorder::Recorder()
{
    auto func = std::bind(&Recorder::hideDragger, this);
    timer = std::make_shared<Timeout>(func);
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
    auto winMain = WindowMain::get();
    if (winMain->state < State::rect)
    {
        return false;
    }
    if (curIndex < 0)
    {
        createShape(x, y, winMain->state);
        curIndex = shapes.size() - 1;
    }
    shapes[curIndex]->OnMouseDown(x, y);
    return false;
}
bool Recorder::OnMouseMove(const int &x, const int &y)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->OnMouseMove(x, y);
        if (flag)
        {
            curIndex = i;
            break;
        }
        else
        {
            curIndex = -1;
        }
    }
    if (curIndex < 0)
    {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        timer->Start();
    }
    return false;
}
bool Recorder::OnMouseDrag(const int &x, const int &y)
{
    if (curIndex >= 0)
    {
        shapes[curIndex]->OnMoseDrag(x, y);
    }
    return false;
}
bool Recorder::OnMouseUp(const int &x, const int &y)
{
    if (curIndex < 0)
    {
        return false;
    }
    if (shapes[curIndex]->IsWIP)
    {
        shapes.erase(shapes.begin() + curIndex, shapes.begin() + 1);
    }
    shapes[curIndex]->OnMouseUp(x, y);
    curIndex = -1;
    return false;
}

bool Recorder::OnPaint(SkCanvas *canvas)
{
    for (auto &shape : shapes)
    {
        shape->OnPaint(canvas);
    }
    return false;
}

void Recorder::hideDragger()
{
    auto win = WindowMain::get();
    if (!win)
        return;
    bool flag = false;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (i != curIndex)
        {
            if (shapes[i]->showDragger)
            {
                shapes[i]->showDragger = false;
                flag = true;
            }
        }
    }
    if (flag)
    {
        PostMessage(WindowMain::get()->hwnd, WM_REFRESH, NULL, NULL);
    }
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
        break;
    }
    case State::mosaic:
    {
        break;
    }
    case State::eraser:
    {
        break;
    }
    default:
        break;
    }
}