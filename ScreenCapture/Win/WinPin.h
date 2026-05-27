#pragma once
#include "pch.h"
#include "WinBase.h"

class WinToolMain;
class WinToolSub;
class ShapeBase;
class WinPin :public WinBase
{
public:
	WinPin(const int& x, const int& y, const int& w, const int& h);
	~WinPin();
	static void init();
	static WinPin* getCur();
public:
private:
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onPaint() override;
	bool onCursor() override;
	void onTimer(const UINT& timerId) override;
	void initImg();
private:
	ComPtr<ID2D1Bitmap> screenImg;
	POINT pressPos;
	std::vector<std::unique_ptr<ShapeBase>> shapes;
	ShapeBase* shapeHover;
};

