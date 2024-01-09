#pragma once

#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include <vector>

class ShapeBase
{
public:
    ShapeBase(const int& x, const int& y);
    ~ShapeBase();
    virtual bool OnMouseDown(const int& x, const int& y) { return false; };
    virtual bool OnMouseMove(const int& x, const int& y) = 0;
    virtual bool OnMouseUp(const int& x, const int& y) = 0;
    virtual bool OnMoseDrag(const int& x, const int& y) { return false; };
    virtual bool OnChar(const unsigned int& val) { return false; };
    virtual bool OnKeyDown(const unsigned int& val) { return false; };
    virtual bool onMouseWheel(const int& delta) { return false; };
    virtual void Paint(SkCanvas* canvas) { };
    bool IsWip{true}; //在制品，在FrontCanvas绘制
    bool IsDel{ false }; //Undo品
    bool IsTemp{ true }; //首次创建，极有可能随时被删除
    int HoverIndex{-1};
protected:
    int startX, startY;
private:
};