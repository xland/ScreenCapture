#include "AppFont.h"
#include <Windows.h>
#include <shlobj.h>
#include <string>
#include <filesystem>
#include "../res/res.h"
#include "Util.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkData.h"

static AppFont *appFont;

AppFont::AppFont()
{
    auto textFace = SkTypeface::MakeFromName("Arial", SkFontStyle::Normal());
    fontText = new SkFont(textFace);

    HMODULE instance = GetModuleHandle(NULL);
    HRSRC resID = FindResource(instance, MAKEINTRESOURCE(IDR_ICON_FONT), L"ICON_FONT");
    if (resID == 0)
    {
        MessageBox(NULL, L"查找字体图标资源失败", L"系统提示", NULL);
        return;
    }
    size_t resSize = SizeofResource(instance, resID);
    HGLOBAL res = LoadResource(instance, resID);
    if (res == 0)
    {
        MessageBox(NULL, L"加载字体图标资源失败", L"系统提示", NULL);
        return;
    }
    LPVOID resData = LockResource(res);
    auto fontData = SkData::MakeWithoutCopy(resData, resSize);
    auto iconFace = SkTypeface::MakeFromData(fontData);
    fontIcon = new SkFont(iconFace);
}
AppFont::~AppFont()
{
    delete fontIcon;
    delete fontText;
}
void AppFont::Init()
{
    if (!appFont)
    {
        appFont = new AppFont();
    }
}

AppFont *AppFont::Get()
{
    return appFont;
}
