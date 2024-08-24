#pragma once
#include <vector>
#include "include/core/SkColor.h"

class Screen
{
public:
	~Screen();
	static void Init();
	std::vector<SkColor> screenPhoto;
	std::vector<RECT> screenRects;
	Screen* Get();
private:
	Screen();
	void shotScreen();
	void getScreenRects();
};