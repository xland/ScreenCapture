#pragma once
#include "include/core/SkRect.h"
#include <vector>
#include <memory>
#include "Windows.h"


class Screen
{
public:
    ~Screen();
    static void Init();
    static std::shared_ptr<SkRect> GetScreen(const float& x, const float& y);
private:
    Screen();
};