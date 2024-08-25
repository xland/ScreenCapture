#pragma once
#include "WinBase.h"
#include <memory>
#include "State.h"

class CutMask;
class ToolMain;
class ToolSub;
class WinMax:public WinBase
{
public:
	WinMax();
	~WinMax();
	static WinMax* Get();
	static void Init();
	State state{State::start};
private:
	void onPaint() override;
	void onLeftBtnDown(const int& x, const int& y) override;
	void onLeftBtnUp(const int& x, const int& y) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y) override;
	std::shared_ptr<CutMask> cutMask;
	std::shared_ptr<ToolMain> toolMain;
	std::shared_ptr<ToolSub> toolSub;
};
