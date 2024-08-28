#pragma once
#include <vector>
#include <Windows.h>
#include "include/core/SkColor.h"
#include "include/core/SkRect.h"

class WinMax;
class Screen
{
public:
	Screen();
	~Screen();
	void Init();
	float GetScreenLeftByPos(const float& x, const float& y);
	std::vector<SkColor> screenPix;
	int x{0}, y{ 0 }, w{ 0 }, h{ 0 };
private:
	std::vector<RECT> screenRects;
	void shotScreen();
	void getScreenRects();
	void initPosSize();
};