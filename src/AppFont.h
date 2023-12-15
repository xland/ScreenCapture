#pragma once
#include "include/core/SkTypeface.h"
class AppFont
{
public:
    ~AppFont();
    static void Init();
    static AppFont *Get();
    sk_sp<SkTypeface> fontIcon;
    sk_sp<SkTypeface> fontText;

private:
    AppFont();
};