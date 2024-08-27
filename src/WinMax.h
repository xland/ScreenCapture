#pragma once
#include "WinBase.h"
#include <memory>

class CutMask;
class Screen;
class WinMax:public WinBase
{
public:
	WinMax();
	~WinMax();
	void Init() override;
	std::unique_ptr<Screen> screen;
	std::unique_ptr<CutMask> cutMask;
private:
	void onPaint() override;
};
