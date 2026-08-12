#pragma once
#include <include/Ling.h>

class TitleBar:public Ling::Node
{
public:
	TitleBar(Ling::WinBase* win);
	~TitleBar();
	LRESULT hitCaption(const POINT pt);
public:
	float height{ 30.f };
protected:
private:
	void onBtnClick(Ling::Button* btn);
private:
	Ling::Node* titleBox;
	std::vector<Ling::Button*> btns;
};
