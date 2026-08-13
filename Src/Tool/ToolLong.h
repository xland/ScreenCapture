#pragma once
#include <include/Ling.h>

class WinCap;
class Tip;
class ToolLong : public Ling::WinBase
{
public:
	ToolLong(WinCap* win);
	~ToolLong();
private:
	void onCreated() override;
	void onClick(Ling::Button* btn);
	void onMinMaxInfo(MINMAXINFO* mmi) override;
	// 按当前 dpi 把窗口尺寸算出来并应用。构造时算一次，DPI 变了再算一次
	void refreshSize();
private:
	WinCap* win;
	// onDpiChanged 与 onSizeChanged 之间的接力标记，见构造函数里的注释
	bool dpiChanged{ false };
	// 逻辑像素，交给 Ling 的 setter 时由其内部乘 dpi
	static constexpr float btnSize{ 32.f };
	std::vector<std::wstring> btnIds = { L"pin",L"close",L"save",L"clipboard" };
	std::vector<std::wstring> btnCodes = { L"\ue6a2",L"\ue62d",L"\ue608",L"\ue6ad" };
	// 悬停提示。要 hwnd，所以在 onCreated 里才建得起来
	std::unique_ptr<Tip> tip;
};
