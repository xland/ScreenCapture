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
    static void init();
    static Recorder *get();
    bool OnMouseDown(const int &x, const int &y);
    bool OnMouseUp(const int &x, const int &y);
    bool OnMouseMove(const int &x, const int &y);
    bool OnMouseDrag(const int& x, const int& y);
    bool OnPaint(SkCanvas *canvas);

private:
    Recorder();
    void createShape(const int& x, const int& y,const State& state);
    std::vector<std::shared_ptr<ShapeBase>> shapes;
    int curIndex{-1};
};