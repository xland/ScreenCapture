#include "pch.h"
#include "App.h"
#include "Setting.h"
#include "Tray.h"
#include "Lang.h"
#include "./Win/WinCap.h"

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
		if (app->args[L"--auto-start"] == L"true") return; //开机自启模式不启动截图
		WinCap::init();//默认情况下，应用启动随即进入截图模式
    }
}
