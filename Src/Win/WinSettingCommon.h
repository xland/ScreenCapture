#pragma once
#include <include/Ling.h>
#include <functional>
class WinSettingCommon:public Ling::Node
{
public:
	WinSettingCommon(Ling::WinBase* parent);
	~WinSettingCommon();
	// 收掉通用下拉框。它挂在 win->body 上而不是挂在本节点里，所以本节点被换掉 / 销毁时
	// 它不会跟着走，得由外面在合适的时机显式收掉
	void hideSelectBox();
private:
	void initAutoStartCtrls();
	void initLangCtrls();
	void initImageReaderCtrls();
	void setAutoStartBtn(Ling::Button* btn);
	void setImageReaderBtn(Ling::Button* btn);
	void showSelectBox(Ling::Button* owner, const std::vector<std::wstring>& items, std::function<void(int)> onSelect);
private:
	Ling::ScrollerBox* selectBox{ nullptr };
	winrt::event_token onMouseDownToken;
};

