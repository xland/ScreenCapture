#pragma once
#include <vector>
#include "include/core/SkRect.h"

class ShapeDragger
{
public:
    ~ShapeDragger();
    static void init();
    static ShapeDragger* get();
    void setDragger(size_t index, float x, float y);
    std::vector<SkRect> draggers;
    bool visible{ false };
    void showDragger();
    void hideDragger();
    int indexMouseAt(const int& x, const int& y);
private:
    ShapeDragger();
    float size{ 10 };
};