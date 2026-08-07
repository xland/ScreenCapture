#pragma once
#include <include/Ling.h>
class WinPin;
class ToolMain : public Ling::WinBase
{
public:
	ToolMain(WinPin* win);
	~ToolMain();
	static void init();
	float getBtnCenterX();
public:
	std::wstring curId;
private:
	void onCreated() override;
	void onClick(Ling::Button* btn);
	void onMinMaxInfo(MINMAXINFO* mmi);
private:
	WinPin* win;
	std::vector<std::wstring> btnIds = { L"rect",L"ellipse",L"arrow",L"number",L"line",L"text",L"mosaic", L"eraser",L"|",L"undo",L"redo",L"|",L"close",L"save",L"clipboard" };
	std::vector<std::wstring> btnCodes = { L"\ue8e8",L"\ue6bc",L"\ue603",L"\ue776",L"\ue601",L"\ue6ec",L"\ue82e",L"\ue6be",L"|",L"\ued85",L"\ued8a",L"|",L"\ue62d",L"\ue608",L"\ue6ad" };
	std::vector<Ling::Button*> btns;
};

