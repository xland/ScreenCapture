#pragma once
#include <include/Ling.h>

class Tray
{
public:
	~Tray();
	static void init();
	static Tray* get();
private:
	Tray();
	void onTrayRightClick();
};
