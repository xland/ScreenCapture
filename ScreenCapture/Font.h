#pragma once
#include "blend2d.h"
class Font
{
public:
	~Font();
	static void Init();
	static void Dispose();
	static Font* Get();
	BLFont* fontIcon;
	BLFont* fontText;
private:
	Font();
	void initFontIcon();
	void initFontText();
};

