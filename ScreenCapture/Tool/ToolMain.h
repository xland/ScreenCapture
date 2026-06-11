#pragma once
#include "pch.h"
#include "ToolBase.h"


class ToolMain :public ToolBase
{
public:
	ToolMain(const int& x, const int& y, const int& w, const int& h, WinPin* parent);
	~ToolMain();
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
	void onDpiChanged() override;
	void onCreated() override;
	void onKeyDown(const UINT& key) override;
private:
	int spliterIndex[2]{ 8,10 };
	WinPin* parent;
};

