
#pragma once
#include "include/core/SkFontMgr.h"
#include "include/core/SkFont.h"
#include <memory>
class Font
{
public:
	Font();
	~Font();
	static void Init();
	static SkFont* GetIcon();
	static SkFont* GetText();
private:
	void initFontIcon(SkFontMgr* fontMgr);
	void initFontText(SkFontMgr* fontMgr);
	std::unique_ptr<SkFont> icon;
	std::unique_ptr<SkFont> text;	
};