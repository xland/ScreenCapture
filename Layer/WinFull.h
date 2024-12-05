#pragma once
#include <Windows.h>

#include "WinBase.h"

class WinFull:public WinBase
{
public:
    WinFull();
    virtual ~WinFull();
protected:
    void paint(SkCanvas* canvas) override;
private:
    void initSize();
    void shotScreen();
    void initCanvas();
};