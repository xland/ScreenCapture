#pragma once
#include "WinBase.h"
#include <memory>

class CutMask;
class ToolMain;
class ToolSub;
class WinMax:public WinBase
{
public:
	WinMax();
	~WinMax();
private:
	void onPaint() override;
	void onLeftBtnDown(const int& x, const int& y) override;
	void onLeftBtnUp(const int& x, const int& y) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y) override;
	void onCustomMsg(const EventType& type, const uint32_t& msg) override;
	std::shared_ptr<CutMask> cutMask;
	std::shared_ptr<ToolMain> toolMain;
	std::shared_ptr<ToolSub> toolSub;
};
