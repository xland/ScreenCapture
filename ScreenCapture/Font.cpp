#include "Font.h"
#include <Windows.h>
#include "resource.h"

static Font* font;

Font::Font()
{
    initFontIcon();
    initFontText();
}
Font::~Font()
{
    delete fontText;
    delete fontIcon;
}
void Font::Dispose()
{
    delete font;
}
Font* Font::Get()
{
    if (!font) {
        font = new Font();
    }
    return font;
}

void Font::initFontText()
{
    BLFontFace face;
    BLResult err = face.createFromFile("C:\\Windows\\Fonts\\simhei.ttf"); //黑体
    if (err) {
        MessageBox(NULL, L"系统字体加载失败", L"系统提示", NULL);
        return;
    }
    fontText = new BLFont();
    fontText->createFromFace(face, 16.0f);
}

void Font::initFontIcon()
{
    HMODULE instance = GetModuleHandle(NULL);
    HRSRC resID = FindResource(instance, MAKEINTRESOURCE(IDR_ICON_FONT), L"ICON_FONT");
    if (resID == 0) {
        MessageBox(NULL, L"查找字体图标资源失败", L"系统提示", NULL);
        return;
    }
    size_t resSize = SizeofResource(instance, resID);
    HGLOBAL res = LoadResource(instance, resID);
    if (res == 0) {
        MessageBox(NULL, L"加载字体图标资源失败", L"系统提示", NULL);
        return;
    }
    LPVOID resData = LockResource(res);
    BLFontData fontData;
    BLResult result = fontData.createFromData(resData, resSize);
    //BOOL flag = VirtualFree(resData, resSize, MEM_RELEASE);
    if (result != BL_SUCCESS) {
        MessageBox(NULL, L"生成字体图标失败", L"系统提示", NULL);
        return;
    }
    BLFontFace face;
    face.createFromData(fontData, 0);
    fontIcon = new BLFont();
    result = fontIcon->createFromFace(face, 28.0f);
    if (result != BL_SUCCESS) {
        MessageBox(NULL, L"创建字体图标失败", L"系统提示", NULL);
        return;
    }
}