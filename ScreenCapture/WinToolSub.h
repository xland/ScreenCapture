#pragma once
#include "pch.h"
#include "WinBase.h"
class WinToolSub :public WinBase
{
public:
	WinToolSub(const int& x, const int& y, const int& w, const int& h);
	~WinToolSub();
public:
protected:
	void onPaint() override;

};

