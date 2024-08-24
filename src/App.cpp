#include "App.h"
#include "Cmd.h"
#include "Font.h"
#include "Screen.h"
#include "Tray.h"
#include <memory>

namespace {
	std::shared_ptr<App> app;
}

App::~App()
{
}

void App::Init(HINSTANCE instance, std::wstring&& cmd)
{
	app = std::shared_ptr<App>{new App()};
	app->instance = instance;
	Cmd::Init(cmd);
	Tray::Init();
	Font::Init();
	Screen::Init();
}

App* App::Get()
{
	return app.get();
}

int App::GetExitCode()
{
	return 0;
}
