#pragma once
#include <include/Ling.h>
class WinSettingCommon:public Ling::Node
{
public:
	WinSettingCommon(Ling::WinBase* parent);
	~WinSettingCommon();
private:
	void initAutoStartCtrls();
	void initLangCtrls();
	void setAutoStartBtn(Ling::Button* btn);
	void showSelectBox(Ling::Button* btn);
private:
	Ling::Button* selectBtn{ nullptr };
	Ling::ScrollerBox* selectBox{ nullptr };
	winrt::event_token onMouseDownToken;
};

