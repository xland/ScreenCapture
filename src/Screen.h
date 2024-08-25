#pragma once
#include <vector>
#include <Windows.h>
#include "include/core/SkColor.h"

class WinMax;
class Screen
{
public:
	~Screen();
	static void Init();
	static Screen* Get();
	void Prepare(WinMax* win);

	std::vector<SkColor> screenPix;
	std::vector<RECT> screenRects;
	int x, y, w, h;
private:
	Screen();
	void shotScreen();
	void getScreenRects();
	void initPosSize();
};