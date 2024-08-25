
#pragma once
#include "include/core/SkFontMgr.h"
#include "include/core/SkFont.h"
#include <memory>
class Font
{
public:
	~Font();
	static void Init();
	static Font* Get();
	std::shared_ptr<SkFont> icon;
	std::shared_ptr<SkFont> text;
private:
	Font();
	void initFontIcon(SkFontMgr* fontMgr);
	void initFontText(SkFontMgr* fontMgr);
};