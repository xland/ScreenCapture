#pragma once
#include <vector>
#include "include/core/SkRect.h"
#include "include/core/SkCanvas.h"
#include "../Cursor.h"

class ShapeBase;
class ShapeDragger
{
public:
    ~ShapeDragger();
    static void Init();
    static ShapeDragger* Get();
    void SetDragger(size_t index, float x, float y);
    void showDragger(SkCanvas* canvas);
    bool hideDragger();
    void disableDragger(const int& startIndex = 0);
    int indexMouseAt(const int& x, const int& y);
    ShapeBase* CurShape;
    float size{ 10 };
    std::vector<SkRect> draggers;
    std::vector<Cursor::cursor> cursors;
    bool visible{ false };
private:
    ShapeDragger();
};