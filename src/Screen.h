#pragma once
#include <vector>
#include <Windows.h>
#include "include/core/SkColor.h"

class WinMax;
class Screen
{
public:
	Screen();
	~Screen();
	void Init();
	std::vector<SkColor> screenPix;
	std::vector<RECT> screenRects;
	int x{0}, y{ 0 }, w{ 0 }, h{ 0 };
private:
	void shotScreen();
	void getScreenRects();
	void initPosSize();
};