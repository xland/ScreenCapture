#include "Font.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkData.h"
#include "include/ports/SkTypeface_win.h"
#include <Windows.h>
#include <shlobj.h>
#include "../res/res.h"
#include "App.h"
#include <memory>

namespace {
    std::unique_ptr<Font> font;
}

void Font::Init()
{
    font = std::make_unique<Font>();
}

Font::Font()
{
    auto fontMgr = SkFontMgr_New_GDI();
    initFontText(fontMgr.get());
    initFontIcon(fontMgr.get());
}

Font::~Font()
{
}

SkFont* Font::GetIcon()
{
    return font->icon.get();
}

SkFont* Font::GetText()
{
    return font->text.get();
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
    icon = std::make_unique<SkFont>(fontMgr->makeFromData(fontData));
    icon->setEdging(SkFont::Edging::kSubpixelAntiAlias);
    icon->setSubpixel(true);
}

void Font::initFontText(SkFontMgr* fontMgr)
{
    text = std::make_unique<SkFont>(fontMgr->matchFamilyStyle("NSimSun", {}));
    text->setEdging(SkFont::Edging::kSubpixelAntiAlias);
    text->setSubpixel(true);
}
