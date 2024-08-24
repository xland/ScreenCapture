#include "Font.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkData.h"
#include "include/ports/SkTypeface_win.h"
#include <Windows.h>
#include <shlobj.h>
#include "../res/res.h"
#include "App.h"

namespace {
    std::shared_ptr<SkFont> fontIcon;
    std::shared_ptr<SkFont> fontText;
}

Font::Font()
{
}

Font::~Font()
{
}

void Font::Init()
{
    auto fontMgr = SkFontMgr_New_GDI();
    initFontText(fontMgr.get());
    initFontIcon(fontMgr.get());
}

void Font::initFontIcon(SkFontMgr* fontMgr)
{
    auto instance = App::Get()->instance;
    HRSRC resID = FindResource(instance, MAKEINTRESOURCE(IDR_ICON_FONT), L"ICON_FONT");
    if (resID == 0) return;
    size_t resSize = SizeofResource(instance, resID);
    HGLOBAL res = LoadResource(instance, resID);
    if (res == 0) return;
    LPVOID resData = LockResource(res);
    auto fontData = SkData::MakeWithoutCopy(resData, resSize);
    fontIcon = std::make_shared<SkFont>(fontMgr->makeFromData(fontData));
}

void Font::initFontText(SkFontMgr* fontMgr)
{
    fontText = std::make_shared<SkFont>(fontMgr->matchFamilyStyle("Microsoft YaHei", {}));
}
