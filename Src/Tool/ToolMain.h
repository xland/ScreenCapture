#pragma once
#include <include/Ling.h>
class WinPin;
class ToolMain : public Ling::WinBase
{
public:
	ToolMain(WinPin* win);
	~ToolMain();
	static void init();
private:
	void onCreated() override;
private:
	WinPin* win;
};

