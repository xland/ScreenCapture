#pragma once
#include <include/Ling.h>

class WinLong;
class Tip;
class ToolLong : public Ling::WinBase
{
public:
	ToolLong(WinLong* win);
	~ToolLong();
private:
	void onCreated() override;
	void onClick(Ling::Button* btn);
	void onMinMaxInfo(MINMAXINFO* mmi) override;
private:
	WinLong* win;
	std::vector<std::wstring> btnIds = { L"pin",L"close",L"save",L"clipboard" };
	std::vector<std::wstring> btnCodes = { L"\ue6a2",L"\ue62d",L"\ue608",L"\ue6ad" };
	// 悬停提示。要 hwnd，所以在 onCreated 里才建得起来
	std::unique_ptr<Tip> tip;
};
