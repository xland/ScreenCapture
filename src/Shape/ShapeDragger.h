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
    void ShowDragger(bool needRefresh=true);
    bool HideDragger();
    void DisableDragger(const int& startIndex = 0);
    int IndexMouseAt(const int& x, const int& y);
    ShapeBase* CurShape;
    float Size{ 10 };
    std::vector<Cursor::cursor> Cursors;
    bool visible{ false };
private:
    ShapeDragger();
    std::vector<SkRect> draggers;
};