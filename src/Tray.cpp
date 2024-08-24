#include "Tray.h"
#include "Cmd.h"

Tray::Tray()
{
}

void Tray::Init()
{
	auto cmd = Cmd::Get();
	auto val = cmd->GetVal(std::wstring(L"shortcut"));
}

Tray::~Tray()
{
}