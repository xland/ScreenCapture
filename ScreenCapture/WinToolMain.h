#pragma once
#include "pch.h"
#include "WinToolBase.h"

class WinToolMain :public WinToolBase
{
public:
	WinToolMain(const int& x, const int& y, const int& w, const int& h);
	~WinToolMain();
	static void popup();
	static WinToolMain* get();
public:
	std::wstring state{ L"rect" };
protected:
private:
	void onPaint() override;
	bool onCursor() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
private:
	int spliterIndex[2]{ 8,10 };
};

