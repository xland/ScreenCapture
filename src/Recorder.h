#pragma once
#include <vector>
#include <memory>
#include "State.h"

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
    bool ProcessText(int x, int y);
    void ProcessText();
    void Reset();
    ShapeBase* CurShape{ nullptr };
    ShapeBase* HoverShape{ nullptr };
private:
    Recorder();
    void createShape(const int& x, const int& y,const State& state);
    void clickShape(ShapeBase* shape,int x, int y);
    std::vector<std::shared_ptr<ShapeBase>> shapes;
    
};