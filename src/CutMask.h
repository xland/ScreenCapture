#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class CutMask
{
public:
    ~CutMask();
    static void Init(int w,int h);
    static CutMask* Get();
    static SkRect GetCutRect();
    bool OnMouseDown(const int& x, const int& y);
    bool OnMouseUp(const int& x, const int& y);
    bool OnMouseMove(const int& x, const int& y);
    bool OnMouseDrag(const int& x, const int& y);
    bool OnPaint(SkCanvas* canvas);
    bool OnKeyDown(const unsigned int& val);
    void EnumWinRects();
    SkRect CutRect;
private:
    CutMask();
    std::vector<SkRect> winRects;
    SkPoint start{-1,-1};
    int hoverIndex{ 4 };
};