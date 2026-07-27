#include "pch.h"
#include "App.h"
#include "Setting.h"
#include "Tray.h"
#include "Lang.h"

App::~App()
{
}

void App::init()
{
    Ling::init();
    auto app = Ling::App::get();
    app->initArgs();
    Ling::D2D::get()->addFonts({ L"icon.ttf" });
    Setting::init();
    Lang::init();
    if (app->args[L"auto-quit"] == L"true") {
        //用完即走模式
    }
    else {
        bool flag = app->refuseSecondInstance();
        if (flag) return;
        Tray::init();
    }
}

App::App()
{
}
