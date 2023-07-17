#include "MainWin.h"
#include "Icon.h"



BLRect MainWin::getToolRect()
{
	int btnCount = 14;
	int span = 8;//工具条距离截图区域的高度
	int toolBtnWidth = 58;
	int toolBtnSpanWidth = 6;
	int toolBtnSpanCount = 2;
	BLRect result;
	result.w = btnCount * toolBtnWidth + toolBtnSpanWidth * toolBtnSpanCount;
	result.h = 56;
	result.x = cutBox.x1 - result.w;
	if (h - cutBox.y1 > span * 2 + result.h) //两个高度，为屏幕底边也留一点间隙 
	{
		result.y = cutBox.y1 + span;
	}
	else if(cutBox.y0 > span * 2 + result.h)
	{
		result.y = cutBox.y0 - span - result.h;
	}
	else
	{
		result.y = cutBox.y1 - span - result.h;
	}
	return result;
}

void MainWin::drawTool()
{
	if (state == State::Start) return;
	if (!fontIcon) {
		initFontIcon();
	}
	auto rect = getToolRect();
	paintCtx->setFillStyle(BLRgba32(240, 240, 240, 255));
	paintCtx->fillRect(rect);
	paintCtx->setFillStyle(BLRgba32(0, 0, 0, 255));
	//const char* c = reinterpret_cast<const char*>(u8"\ue7c2");
	//auto c = Icon::icons[Icon::Name::rectEllipse];
	const char* icons[] = {
		reinterpret_cast<const char*>(u8"\ue790"),
		reinterpret_cast<const char*>(u8"\ue791"),
		reinterpret_cast<const char*>(u8"\ueb0c"),
		reinterpret_cast<const char*>(u8"\ue601"),
		reinterpret_cast<const char*>(u8"\ue69b"),
		reinterpret_cast<const char*>(u8"\ue82e"),
		reinterpret_cast<const char*>(u8"\ue6ec"),
		reinterpret_cast<const char*>(u8"\ue776"),
		reinterpret_cast<const char*>(u8"\ue6b8"),


		reinterpret_cast<const char*>(u8"\ued85"),
		reinterpret_cast<const char*>(u8"\ued8a"),

		reinterpret_cast<const char*>(u8"\ue62f"),
		reinterpret_cast<const char*>(u8"\ue87f"),
		reinterpret_cast<const char*>(u8"\ue6e7"),
	};
	BLPoint point;
	point.y = rect.y + 38;
	point.x = rect.x + 20;
	for (size_t i = 0; i < std::size(icons); i++)
	{
		paintCtx->fillUtf8Text(point, *fontIcon, icons[i]); 
		point.x += 58;
	}	
}