#pragma once
#include <include/Ling.h>

class WinSetting :public Ling::WinBase
{
public:
	~WinSetting();
	static void init();
	// 退出流程里调：窗口对象是文件级静态变量，交给静态析构就在 CoUninitialize 之后了
	static void dispose();
private:
	WinSetting();
	void initMenuItems(Ling::Node* menuBox);
	void onCreated() override;
	void onMenuItemClick(Ling::Button* menu);
	LRESULT onHitTest(const POINT pos) override;
private:
	std::vector<Ling::Button*> menus;
	int menuIndex{ 0 };
	Ling::Node* content{nullptr};
};

