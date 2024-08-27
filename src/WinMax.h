#pragma once
#include "WinBase.h"
#include <memory>

class CutMask;
class ToolMain;
class ToolSub;
class Screen;
class WinMax:public WinBase
{
public:
	WinMax();
	~WinMax();
	void Init() override;
private:
	void onPaint() override;
	std::unique_ptr<Screen> screen;
	std::unique_ptr<CutMask> cutMask;
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
};
