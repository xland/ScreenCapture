#pragma once
#include "include/core/SkCanvas.h"
class EventHandler
{
public:
    EventHandler();
    ~EventHandler();
    virtual bool OnMouseDown(const int& x, const int& y) { return false; };
    virtual bool OnMouseUp(const int& x, const int& y) { return false; };
    virtual bool OnMouseMove(const int& x, const int& y) { return false; };
    virtual bool OnDrag(const int& x, const int& y) { return false; };
    virtual bool OnPaint(SkCanvas *canvas) { return false; };

protected:
private:
};