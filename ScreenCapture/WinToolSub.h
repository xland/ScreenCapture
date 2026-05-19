#pragma once
#include "pch.h"
#include "WinToolBase.h"
class WinToolSub :public WinToolBase
{
public:
	WinToolSub();
	~WinToolSub();
	void show() override;
public:
protected:
	void onPaint() override;
	void onClick() override;

};

