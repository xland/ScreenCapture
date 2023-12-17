#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "EventHandler.h"
#include <vector>

class ShapeBase;
class Recorder : public EventHandler
{
public:
    ~Recorder();
    static void init();
    static Recorder *get();
    bool OnMouseDown(const int& x, const int& y) override;
    bool OnMouseUp(const int& x, const int& y) override;
    bool OnMouseMove(const int& x, const int& y) override;
    bool OnPaint(SkCanvas *canvas) override;

private:
    Recorder();
    std::vector<std::shared_ptr<ShapeBase>> shapes;
};