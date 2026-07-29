#pragma once
#include <include/Ling.h>
class WinSettingShortcut :public Ling::Node
{
public:
	WinSettingShortcut(Ling::WinBase* parent);
	~WinSettingShortcut();
private:
	void onBtnClick(Ling::Button* btn);
	void resetCurKey();
	std::vector<Ling::Button*> btns;
	std::wstring curKey;
	winrt::event_token onMouseDownToken;
	std::shared_ptr<bool> alive{ std::make_shared<bool>(true) };
};

