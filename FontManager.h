#pragma once

#include <include/core/SkFont.h>
class FontManager
{
public:
	static void Init();
	static SkFont* Icon();
	static SkFont* Text();
private:
	FontManager() {};
	~FontManager() {};
};