#pragma once
#include "WindowBase.h"
class PinWin : public WindowBase
{
public:
	PinWin(const int& x,const int& y, const int& w, const int& h, BLImage* img);
	~PinWin();
private:
	int OnHitTest(const int& x, const int& y) override;
	void SetToolMainPos() override;
	void drawSrcImg();
	void drawShadow();
	BLImage* srcImg;
};

