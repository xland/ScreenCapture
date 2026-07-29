#include "pch.h"
#include "App.h"
#include "Setting.h"
#include "Tray.h"
#include "Lang.h"

std::unique_ptr<App> app;


App::~App()
{
}

void App::init()
{
    auto ptr = new App();
    app.reset(ptr);
}

App* App::get()
{
    return app.get();
}

App::App()
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
