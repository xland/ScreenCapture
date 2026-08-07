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
	// \u4ec5\u7ed8\u56fe\u7c7b\u6309\u94ae\u53ef\u88ab\u9009\u4e2d\u5e76\u5199\u5165 curId\uff0c\u52a8\u4f5c\u7c7b\u6309\u94ae\u70b9\u51fb\u540e\u4e0d\u6539\u53d8\u9009\u4e2d\u72b6\u6001
	std::vector<std::wstring> drawIds = { L"rect",L"ellipse",L"arrow",L"number",L"line",L"text",L"mosaic",L"eraser" };
	std::vector<Ling::Button*> btns;
};

