#include <Windows.h>
#include <include/core/SkFont.h>
#include <include/core//SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/core/SkData.h>
#include <include/ports/SkTypeface_win.h>

#include "FontManager.h"
#include "Resource/res.h"

namespace {
    std::unique_ptr<SkFont> fontIcon{ nullptr };
    std::unique_ptr<SkFont> fontText{ nullptr };
}

void FontManager::Init()
{
    HMODULE instance = GetModuleHandle(NULL);
    HRSRC resID = FindResource(instance, MAKEINTRESOURCE(IDR_ICON_FONT), L"ICON_FONT");
    if (resID == 0) return;
    size_t resSize = SizeofResource(instance, resID);
    HGLOBAL res = LoadResource(instance, resID);
    if (res == 0) return;
    LPVOID resData = LockResource(res);
    auto fontData = SkData::MakeWithoutCopy(resData, resSize);
    sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_GDI();
    fontIcon = std::make_unique<SkFont>(fontMgr->makeFromData(fontData));
    fontText = std::make_unique<SkFont>(fontMgr->matchFamilyStyle("Microsoft YaHei", {}));
}

SkFont* FontManager::Icon()
{
	return fontIcon.get();
}

SkFont* FontManager::Text()
{
	return fontText.get();
}
