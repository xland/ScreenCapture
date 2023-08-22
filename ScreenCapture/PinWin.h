#pragma once
#include "WindowBase.h"
class PinWin : public WindowBase
{
public:
	PinWin(const int& x,const int& y,BLImage* img);
	~PinWin();
private:
	int OnHitTest() override;
};

