#include "App.h"
#include <memory>
#include "Cmd.h"
#include "Font.h"
#include "Screen.h"
#include "Tray.h"
#include "WinMax.h"
#include "WinPin.h"
#include "Lang.h"

namespace {
	std::shared_ptr<App> app;
}
App::~App()
{
}
App::App()
{
}
void App::Init(HINSTANCE instance, std::wstring&& cmd)
{
	app = std::shared_ptr<App>{new App()};
	app->instance = instance;
	Cmd::Init(cmd);
	Lang::Init();
	Tray::Init();
	Font::Init();
	Screen::Init();
	app->win = std::make_shared<WinMax>();
}

App* App::Get()
{
	return app.get();
}

WinBase* App::GetWin()
{
	return app->win.get();
}

int App::GetExitCode()
{
	return 0;
}

