#include "pch.h"
#include "../Lang.h"
#include "../Setting.h"
#include "WinSetting.h"
#include "WinSettingCommon.h"

WinSettingCommon::WinSettingCommon(Ling::WinBase* parent):Ling::Node(parent)
{    
    initAutoStartCtrls();
    initLangCtrls();
    initImageReaderCtrls();
    auto weakThis = getWeakThis();
    // 这个回调一直挂在窗口上，而本节点可能在窗口关闭之前就被菜单切换换掉了，
    // 所以先确认自己还活着再去碰成员
    win->onDestroy.add([this, weakThis]() {
        if (!weakThis.lock()) return;
        this->hideSelectBox();
    });
}

WinSettingCommon::~WinSettingCommon()
{
    win->onMouseDown.remove(onMouseDownToken);
}

void WinSettingCommon::initAutoStartCtrls()
{
    auto box = makeChild<Ling::Node>();
    box->setHeight(39.f);
    box->setFlexDirection(Ling::FlexDirection::Row);
    box->setAlignItems(Ling::Align::Center);

    auto label = box->makeChild<Ling::Label>();
    label->setText(Lang::get(L"setting.autoStart"));
    label->setHeightPercent(100.f);
    label->setJustifyContent(Ling::Justify::Center);
    label->setFlexGrow(1.f);

    auto btn = box->makeChild<Ling::Button>();
    btn->setText(L"\ue687");
    btn->setFontFamily(L"icon");
    btn->setHeightPercent(100.f);
    btn->setFontSize(18.f);
    btn->setWidth(60.f);
    setAutoStartBtn(btn);

    btn->onClick.add([this](Ling::Button* btn) {
        auto setting = Setting::get();
        auto isAutoStart = setting->getAutoStart();
        setting->setAutoStart(!isAutoStart);
        setAutoStartBtn(btn);
    });

    auto border = makeChild<Ling::Node>();
    border->setHeight(1.f);
    border->setBg(0xE0E0E0FF);
}

void WinSettingCommon::initLangCtrls()
{
    auto box = makeChild<Ling::Node>();
    box->setHeight(39.f);
    box->setFlexDirection(Ling::FlexDirection::Row);
    box->setAlignItems(Ling::Align::Center);

    auto label = box->makeChild<Ling::Label>();
    label->setText(Lang::get(L"setting.language"));
    label->setHeightPercent(100.f);
    label->setJustifyContent(Ling::Justify::Center);
    label->setFlexGrow(1.f);

    auto langCode = Setting::get()->getLang();
    auto langs = Lang::get()->getSupportedLang();
    std::wstring langName{ L"简体中文" };
    for (auto& pair:langs)
    {
        if (pair.second == langCode) {
            langName = pair.first;
            break;
        }
    }
    auto selectBtn = box->makeChild<Ling::Button>();
    selectBtn->setText(langName);
    selectBtn->setHeight(28.f);
    selectBtn->setWidth(160.f);
    selectBtn->setBorder(1.f, 0xE0E0E0FF);
    selectBtn->setHoverBg(0XFFFFFFFF);
    selectBtn->onClick.add([this](Ling::Button* btn) {
        if (selectBox) return;

        auto langList = Lang::get()->getSupportedLang();

        std::vector<std::wstring> items;
        items.reserve(langList.size() + 1);
        for (auto& pair : langList) items.push_back(pair.first);
        items.push_back(Lang::get(L"setting.getMoreLang"));

        this->showSelectBox(btn, items, [this, langList](int index) {
            if (index >= static_cast<int>(langList.size())) {
                std::wstring downloadUrl{ L"https://github.com/xland/ScreenCapture/tree/main/Lang" };
                ShellExecute(this->win->hwnd, L"open", downloadUrl.data(), nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }

            Setting::get()->setLang(langList[index].second);
            this->win->close();
            Ling::App::get()->dq.TryEnqueue([this]() {
                WinSetting::init();
            });
        });
    });
    auto border = makeChild<Ling::Node>();
    border->setHeight(1.f);
    border->setBg(0xE0E0E0FF);
}

void WinSettingCommon::initImageReaderCtrls()
{
    auto box = makeChild<Ling::Node>();
    box->setHeight(39.f);
    box->setFlexDirection(Ling::FlexDirection::Row);
    box->setAlignItems(Ling::Align::Center);

    auto label = box->makeChild<Ling::Label>();
    label->setText(Lang::get(L"setting.imageReader"));
    label->setHeightPercent(100.f);
    label->setJustifyContent(Ling::Justify::Center);
    label->setFlexGrow(1.f);

    auto btn = box->makeChild<Ling::Button>();
    btn->setHeight(28.f);
    btn->setWidth(160.f);
    btn->setBorder(1.f, 0xE0E0E0FF);
    btn->setHoverBg(0XFFFFFFFF);
    setImageReaderBtn(btn);

    btn->onClick.add([this](Ling::Button* btn) {
        if (selectBox) return;
        std::vector<std::wstring> items{ Lang::get(L"setting.imageReaderAuto"), Lang::get(L"setting.imageReaderBrowse") };
        this->showSelectBox(btn, items, [this, btn](int index) {
            if (index == 0) {
                Setting::get()->setImageReaderPath(L"");
                this->setImageReaderBtn(btn);
                return;
            }

            auto typeName = Lang::get(L"util.file");
            COMDLG_FILTERSPEC filterSpec[]{ { typeName.c_str(), L"*.exe" } };
            auto path = this->win->openFileDialog(filterSpec);
            if (path.empty()) return;
            Setting::get()->setImageReaderPath(path);
            this->setImageReaderBtn(btn);
        });
    });

    auto border = makeChild<Ling::Node>();
    border->setHeight(1.f);
    border->setBg(0xE0E0E0FF);
}

void WinSettingCommon::setAutoStartBtn(Ling::Button* btn)
{
    auto setting = Setting::get();
    auto isAutoStart = setting->getAutoStart();
    if (isAutoStart) {
        btn->setText(L"\ue688");
        btn->setColor(0x597ef7ff);
        btn->setHoverColor(0x597ef7ff);
    }
    else {
        btn->setText(L"\ue687");
        btn->setColor(0x666666FF);
        btn->setHoverColor(0x666666FF);
    }
}

void WinSettingCommon::setImageReaderBtn(Ling::Button* btn)
{
    auto path = Setting::get()->getImageReaderPath();
    btn->setText(path.empty() ? Lang::get(L"setting.imageReaderAuto") : std::filesystem::path{ path }.filename().wstring());
}

void WinSettingCommon::hideSelectBox()
{
    if (!selectBox) return;
    win->onMouseDown.remove(onMouseDownToken);
    win->body->removeChild(selectBox);
    selectBox = nullptr;
}

void WinSettingCommon::showSelectBox(Ling::Button* owner, const std::vector<std::wstring>& items, std::function<void(int)> onSelect)
{
    if (selectBox) hideSelectBox();

    auto weakThis = getWeakThis();
    onMouseDownToken = win->onMouseDown.add([this, weakThis, owner](POINT pos, bool isRight) {
        if (!weakThis.lock()) return;
        if (!this->selectBox) return;
        if (owner->isPosIn(pos)) return;
        if (this->selectBox->isPosIn(pos)) return;
        this->hideSelectBox();
    });

    auto itemH{ 30.f };
    auto totalH = std::min(320.f, itemH * items.size());

    selectBox = win->body->makeChild<Ling::ScrollerBox>();
    selectBox->setSize(owner->w/win->dpi, totalH);
    selectBox->setPositionType(Ling::Position::Absolute);
    selectBox->setPosition(Ling::Edge::Left, owner->x/win->dpi);
    selectBox->setPosition(Ling::Edge::Top, owner->y/win->dpi);
    selectBox->setBg(0xFFFFFFFF);
    selectBox->setBorder(1.f, 0x597ef766);
    for (size_t i = 0; i < items.size(); i++)
    {
        auto item = selectBox->makeChild<Ling::Button>();
        item->setText(items[i]);
        item->setHeight(itemH);
        item->setWidthPercent(100.f);
        item->setHoverBg(0Xf2f2f2FF);
        item->setHoverColor(0X000000FF);
        item->onClick.add([this, i, onSelect](Ling::Button*) {
            onSelect(static_cast<int>(i));
            this->hideSelectBox();
        });
    }
}
