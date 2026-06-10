#pragma once
#include "pch.h"
#include "WinToolBase.h"


class WinToolMain :public WinToolBase
{
public:
	WinToolMain(const int& x, const int& y, const int& w, const int& h, WinPin* parent);
	~WinToolMain();
	void initBtn();
public:
	std::string state;
protected:
private:
	void onPaint() override;
	BOOL onCursor() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
private:
	int spliterIndex[2]{ 8,10 };
};

