#pragma once
#include <include/Ling.h>

class WinCap;
class Tip;
// 框选完成后出现在选区右下方的工具条。往右走是图像标记 / 截长图 / 屏幕录制 /
// 文字识别 / 二维码识别，竖线之后是关闭 / 存文件 / 存剪切板。
class ToolCap : public Ling::WinBase
{
public:
	ToolCap(WinCap* win);
	~ToolCap();
private:
	void onCreated() override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
	void onClick(Ling::Button* btn);
private:
	WinCap* win;
	// 悬停提示。要 hwnd，所以在 onCreated 里才建得起来
	std::unique_ptr<Tip> tip;
	// 三张表一一对应。spliter 是竖线分隔符，不是按钮
	std::vector<std::wstring> btnIds = { L"mark",L"long",L"video",L"ocr",L"qrcode",L"spliter",L"close",L"save",L"clipboard" };
	std::vector<std::wstring> btnCodes = { L"\ue97f",L"\ue73e",L"\ue660",L"\ue67b",L"\ue71e",L"",L"\ue62d",L"\ue608",L"\ue6ad" };
	// 翻译 key。空串是分隔符，没提示可挂
	std::vector<std::wstring> btnTips = { L"cap.mark",L"cap.long",L"cap.video",L"cap.ocr",L"cap.qrcode",L"",L"tool.close",L"tool.save",L"tool.clipboard" };
	// 以下都是逻辑像素，交给 Ling 的 setter 时由其内部乘 dpi
	static constexpr float btnSize{ 32.f };
	static constexpr float spliterW{ 1.f };
};
