#pragma once
#include <include/Ling.h>
class WinPin : public Ling::WinBase
{
public:
	WinPin();
	static void init(int x, int y, int w, int h);
private:
	~WinPin();
};

