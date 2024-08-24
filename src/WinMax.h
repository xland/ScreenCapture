#pragma once
#include "WinBase.h"

class WinMax:public WinBase
{
public:
	WinMax();
	~WinMax();
	void initSize();
	static void Init();
private:
	LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
};
