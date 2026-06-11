#include "pch.h"
#include "WinToolBase.h"
#include "Win/WinPin.h"
#include "History.h"
#include "App.h"

WinToolBase::WinToolBase(const int& x, const int& y, const int& w, const int& h, WinPin* parent) : WinBase(x, y, w, h),parent{parent}
{

}


WinToolBase::~WinToolBase()
{

}

void WinToolBase::initBrush()
{
    render->CreateSolidColorBrush(D2D1::ColorF(0xffffff), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0xe6f4ff), brushSelect.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x333333), brushIcon.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBlue.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x888888), brushSpliter.GetAddressOf());
}

void WinToolBase::paintIcon(const float& posX, IDWriteTextLayout* icon, bool isHover, bool isSelected)
{
    D2D1_POINT_2F origin = { posX, marginTop };
    if (isSelected) {
        float paddingTopBottom{ 4.f * dpi }, paddingLeftRight{ 2.6f * dpi };
        D2D1_ROUNDED_RECT rr = { { posX + paddingLeftRight, paddingTopBottom+marginTop, posX + btnSize - paddingLeftRight, h - paddingTopBottom }, 8, 8 };
        render->FillRoundedRectangle(rr, brushSelect.Get());
        render->DrawTextLayout(origin, icon, brushBlue.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    else {
        auto brush = isHover ? brushBlue.Get() : brushIcon.Get();
        render->DrawTextLayout(origin, icon, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

IDWriteTextLayout* WinToolBase::getBtnIconLayout(const std::string& name)
{
    auto it = std::ranges::find(btnId, name);
    if (it != btnId.end()) {
        auto index = it - btnId.begin();
        return btnLayout[index].Get();
    }
    return nullptr;
}

void WinToolBase::initTip()
{
    INITCOMMONCONTROLSEX iccex = { sizeof(iccex), ICC_BAR_CLASSES };
    InitCommonControlsEx(&iccex);
    tipHwnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd,
        NULL,
        App::get()->hInstance,
        NULL
    );
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = hwnd;
    ti.hinst = App::get()->hInstance;
    ti.uId = 0;
    ti.lpszText = (LPWSTR)L" ";
    ti.rect = { 0, 0, 0, 0 };
    SendMessage(tipHwnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    SendMessage(tipHwnd, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(0, 0));
    //SetWindowPos(tipHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
void WinToolBase::addIconLayout(const std::wstring& icon)
{
    auto writer = App::getWriter();
    static ComPtr<IDWriteTextFormat> iconFormat;
	if (!iconFormat.Get()) {
        //加载内置图标字体
        HRSRC hRes = FindResource(NULL, L"iconfont.ttf", RT_RCDATA);
        if (!hRes) return;
        HGLOBAL hData = LoadResource(NULL, hRes);
        if (!hData) return;
        void* pData = LockResource(hData);
        DWORD size = SizeofResource(NULL, hRes);
        ComPtr<IDWriteInMemoryFontFileLoader> loader;
        writer->CreateInMemoryFontFileLoader(loader.GetAddressOf());
        writer->RegisterFontFileLoader(loader.Get());
        ComPtr<IDWriteFontFile> fontFile;
        loader->CreateInMemoryFontFileReference(writer, pData, size, nullptr, fontFile.GetAddressOf());
        ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
        writer->CreateFontSetBuilder(fontSetBuilder.GetAddressOf());
        fontSetBuilder->AddFontFile(fontFile.Get());
        ComPtr<IDWriteFontSet> fontSet;
        fontSetBuilder->CreateFontSet(fontSet.GetAddressOf());
        ComPtr<IDWriteFontCollection1> fontCollection;
        writer->CreateFontCollectionFromFontSet(fontSet.Get(), fontCollection.GetAddressOf());
        writer->CreateTextFormat(L"icon", fontCollection.Get(),
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            12.f, L"", iconFormat.GetAddressOf());
	}
    ComPtr<IDWriteTextLayout> layout;
    writer->CreateTextLayout(icon.data(), icon.length(), iconFormat.Get(), btnSize, btnSize, layout.GetAddressOf());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(14.f * dpi, { 0, static_cast<UINT32>(icon.length()) });
	btnLayout.push_back(std::move(layout));

}
void WinToolBase::showTipAt(int x, int y)
{
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = hwnd;
    ti.hinst = App::get()->hInstance;
    ti.uId = 0;
    ti.lpszText = (LPWSTR)tipText.c_str();
    // 更新文本
    SendMessage(tipHwnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    // 先用临时位置激活 tooltip，让系统计算尺寸
    SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(x-30.f, y-18.6*dpi));
    SendMessage(tipHwnd, TTM_ACTIVATE, TRUE, 0);
    SendMessage(tipHwnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    // 获取 tooltip 实际尺寸
    RECT rc = { 0 };
    GetWindowRect(tipHwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int centerX = x - width / 2;
    int centerY = y - height;
    SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(centerX, centerY));
}
void WinToolBase::hideTip()
{
    TOOLINFOW ti = { 0 };
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.hwnd = hwnd;
    ti.uId = 0;
    SendMessage(tipHwnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    SendMessage(tipHwnd, TTM_ACTIVATE, FALSE, 0);
    tipText = L"";
}
void WinToolBase::onKeyDown(const UINT& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->redo();
    }
    else if (key == 'C' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->copyToClipboard();
    }
    else if (key == 'S' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->saveToFile();
    }
    else if (key == VK_DELETE)
    {
        parent->history->removeHoverShape();
    }
}