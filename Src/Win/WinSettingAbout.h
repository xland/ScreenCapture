#pragma once
#include <include/Ling.h>
class WinSettingAbout:public Ling::Node
{
public:
	WinSettingAbout(Ling::WinBase* parent);
	~WinSettingAbout();
private:
	std::vector<Ling::Button*> btns;
};

