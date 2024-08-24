
#pragma once
#include "include/core/SkFontMgr.h"
class Font
{
public:
	~Font();
	static void Init();
private:
	Font();
	static void initFontIcon(SkFontMgr* fontMgr);
	static void initFontText(SkFontMgr* fontMgr);
};