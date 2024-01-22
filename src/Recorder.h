#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "State.h"
#include <vector>

class ShapeBase;
class Recorder
{
public:
    ~Recorder();
    static void Init();
    static Recorder *Get();
    bool OnMouseDown(const int &x, const int &y);
    bool OnMouseDownRight(const int& x, const int& y);
    bool OnMouseUp(const int &x, const int &y);
    bool OnMouseMove(const int &x, const int &y);
    bool OnMouseDrag(const int& x, const int& y);
    bool OnChar(const unsigned int& val);
    bool OnKeyDown(const unsigned int& val);
    bool OnMouseWheel(const int& delta);
    bool OnTimeout(const unsigned int& id);
    void Undo();
    void Redo();
    void FinishPaint();
    void Reset();
    ShapeBase* CurShape{ nullptr };
    ShapeBase* HoverShape{ nullptr };
private:
    Recorder();
    void createShape(const int& x, const int& y,const State& state);
    std::vector<std::shared_ptr<ShapeBase>> shapes;
    
};