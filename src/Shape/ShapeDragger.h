#pragma once
#include <vector>
#include "include/core/SkRect.h"
#include "../Icon.h"

class ShapeDragger
{
public:
    ~ShapeDragger();
    static void init();
    static ShapeDragger* get();
    void setDragger(size_t index, float x, float y);
    void showDragger(const int& shapeIndex);
    void hideDragger();
    int indexMouseAt(const int& x, const int& y);
    int shapeIndex{ -1 };
    float size{ 10 };
    std::vector<SkRect> draggers;
    std::vector<Icon::cursor> cursors;
    bool visible{ false };
private:
    ShapeDragger();
};