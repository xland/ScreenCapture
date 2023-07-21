#pragma once
#include "blend2d.h"
class Font
{
public:
	Font();
	~Font();
	static Font* Get();
	BLFont* fontIcon;
	BLFont* fontText;
private:
	void initFontIcon();
	void initFontText();
};

