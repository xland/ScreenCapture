#pragma once
#include "include/core/SkFont.h"
class AppFont
{
public:
    ~AppFont();
    static void Init();
    static AppFont *Get();
    SkFont* fontIcon;
    SkFont* fontText;
    SkScalar fontIconHeight;

private:
    AppFont();
};