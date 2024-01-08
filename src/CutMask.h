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
    static void init();
    static CutMask* get();
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
    void setPath();
    std::vector<SkRect> winRects;
    SkPoint start{-1,-1};
    SkPath path;
    int hoverIndex{ -1 };
};