#pragma once
#include "pch.h"
#include "WinToolBase.h"
class WinToolSub :public WinToolBase
{
public:
	WinToolSub(const int& x, const int& y, const int& w, const int& h);
	~WinToolSub();
	static void popup();
	static WinToolSub* get();
	void changeState();

public:
protected:
	void onPaint() override;
private:
	bool onCursor() override;
	void paintBorder();
	static void getXW(int& x,int& w,float& arrowX);
private:
	float arrowX, marginTop;
};

