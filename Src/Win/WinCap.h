#pragma once
#include <include/Ling.h>

class WinCap:public Ling::WinBase
{
public:
	~WinCap();
	static void init();
private:
	WinCap();
};

