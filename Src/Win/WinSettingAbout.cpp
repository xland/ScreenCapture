#include "pch.h"
#include "../Lang.h"
#include "WinSetting.h"
#include "WinSettingAbout.h"

WinSettingAbout::WinSettingAbout(Ling::WinBase* parent):Ling::Node(parent)
{
    std::vector<std::wstring> keys = { L"version",L"project",L"author" };
    for (auto& key : keys)
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(Lang::get(L"about." + key));
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setId(key);
        if (key == L"version") {
            btn->setText(getVer());
        }
        else if (key == L"project") {
            btn->setText(L"github.com/xland/ScreenCapture");
            btn->setColor(0x597ef7ff);
            btn->setHoverColor(0x597ef7ff);
            btn->onClick.add([this](Ling::Button* btn) {
                std::wstring downloadUrl{ L"https://github.com/xland/ScreenCapture" };
                ShellExecute(win->hwnd, L"open", downloadUrl.data(), nullptr, nullptr, SW_SHOWNORMAL);
                });
        }
        else {
            btn->setText(Lang::get(L"about.wechat"));
            btn->setColor(0x597ef7ff);
            btn->setHoverColor(0x597ef7ff);
            btn->onClick.add([this](Ling::Button* btn) {
                Ling::Util::setTextToClipboard(L"liulun_007");
                MessageBox(win->hwnd, Lang::get(L"about.copySuccess").data(), Lang::get(L"about.sysTip").data(), MB_OK | MB_ICONINFORMATION);
                });
        }
        btn->setAlignItems(Ling::Align::FlexEnd);
        btn->setHeight(28.f);
        btn->setWidth(120.f);
        btn->setBg(0);
        btn->setHoverBg(0);
        btns.push_back(btn);

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }
}

WinSettingAbout::~WinSettingAbout()
{

}

std::wstring WinSettingAbout::getVer()
{
    std::vector<wchar_t> exePath(MAX_PATH);
    //获取exe文件的路径（自身路径）
    if (GetModuleFileName(nullptr, exePath.data(), static_cast<DWORD>(exePath.size())) == 0) {
        return L"0.0.0";
    }
    DWORD dummy;
    //获取版本资源大小
    DWORD versionSize = GetFileVersionInfoSize(exePath.data(), &dummy);
    if (versionSize == 0) {
        return L"0.0.0";
    }
    std::vector<BYTE> versionData(versionSize);
    //获取版本信息
    if (!GetFileVersionInfo(exePath.data(), 0, versionSize, versionData.data())) {
        return L"0.0.0";
    }
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT fileInfoSize = 0;
    if (!VerQueryValue(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &fileInfoSize)) {
        return L"0.0.0";
    }
    int major = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
    int minor = fileInfo->dwFileVersionMS & 0xFFFF;
    int patch = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
    return std::format(L"{}.{}.{}", major, minor, patch);
}
