#pragma once
#include "WindowBase.h"
class PinWin : public WindowBase
{
public:
	PinWin(const int& x,const int& y,  BLImage* img);
	~PinWin();
private:
	int OnHitTest(const int& x, const int& y) override;
	void BeforePaint() override;
	//void drawSrcImg();
	void drawShadow();
	BLImage* srcImg;
};

