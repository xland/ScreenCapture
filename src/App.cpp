#include "App.h"
#include <Windows.h>
#include <shlobj.h>
#include "../res/res.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkData.h"
#include "WindowMain.h"


SkFont* fontIcon{ nullptr };
SkFont* fontText{ nullptr };
WindowBase* winMain{ nullptr };
WindowBase* winPin{ nullptr };
int exitCode{ -1 };//todo


App::~App()
{
}

void App::Init()
{
}

WindowBase* App::GetWinMain()
{
	return nullptr;
}

WindowBase* App::GetWinPin()
{
	return nullptr;
}

SkFont* App::GetFontIcon()
{
	return fontIcon;
}

SkFont* App::GetFontText()
{
	return fontText;
}

void App::initFontIcon()
{
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

void App::initFontText()
{
	auto fontFace = SkTypeface::MakeFromName("Microsoft YaHei", SkFontStyle::Normal());
	fontText = new SkFont(fontFace);
}
