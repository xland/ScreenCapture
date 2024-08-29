#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include <vector>
class ShapeBase
{
public:
    ShapeBase(const int& x, const int& y);
    ~ShapeBase();
    virtual bool OnMouseDown(const int& x, const int& y) { return false; };
    virtual bool OnMouseMove(const int& x, const int& y) { return false; };
    virtual bool OnMouseUp(const int& x, const int& y) { return false; };
    virtual bool OnMoseDrag(const int& x, const int& y) { return false; };
    virtual bool OnChar(const unsigned int& val) { return false; };
    virtual bool OnKeyDown(const unsigned int& val) { return false; };
    virtual bool OnMouseWheel(const int& delta) { return false; };
    virtual void ShowDragger();
    virtual void HideDragger();
    virtual void OnTimeout() {};
    virtual void OnShowDragger(SkCanvas* canvas) {};
    virtual void Paint(SkCanvas* canvas) { };
    bool MouseInDragger(const int& x, const int& y);
    bool IsWip{ true }; //在制品，在FrontCanvas绘制
    bool IsDel{ false }; //Undo品
    bool IsTemp{ true }; //首次创建，极有可能随时被删除
    bool IsDraggerVisible{ false };
    int hoverIndex{ -1 };
    std::vector<SkRect> draggers;
protected:
    int startX, startY;
    int draggerSize{ 12 };
private:
};