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
    fontText = SkTypeface::MakeFromName("Arial", SkFontStyle::Normal());

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
    fontIcon = SkTypeface::MakeFromData(fontData);

    // paint->setTypeface(fontIcon);
}
AppFont::~AppFont()
{
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
