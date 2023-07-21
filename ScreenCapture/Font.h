#pragma once
#include "blend2d.h"
class Font
{
public:
	Font();
	~Font();
	BLFont* fontIcon;
	BLFont* fontText;
private:
	void initFontIcon();
	void initFontText();
};

