#pragma once
#include "pch.h"
#include "WinToolBase.h"

class WinToolMain :public WinToolBase
{
public:
	WinToolMain();
	~WinToolMain();
	void show() override;
public:
protected:
	void onPaint() override;
	void onClick() override;
private:
	IDWriteTextLayout* getIconLayout(const std::wstring& icon);
private:

};

