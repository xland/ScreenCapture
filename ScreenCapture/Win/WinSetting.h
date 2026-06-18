#pragma once
#include "pch.h"
#include "WinBase.h"

/// <summary>
/// 基本设置：开机自启动、应用语言、是否显示托盘图标
/// 快捷键：截图、截长图、录屏
/// 关于应用：作者、赞助、微信群
/// </summary>
class WinSetting :public WinBase
{
public:
	WinSetting(const int& x, const int& y, const int& w, const int& h);
	~WinSetting();
	static void init();
private:
	void onCreated() override;
	void onPaint() override;
	BOOL onCursor() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onDpiChanged() override;
	void onKeyDown(const UINT& key) override;
	void paintCommon();
	void paintShortcut();
	void paintAbout();
private:
	POINT pressPos{ 0,0 };
	int indexHover{ -1 }, menuIndexSelect{0};
	std::vector<ComPtr<IDWriteTextLayout>> menuLabels;
	ComPtr<IDWriteTextLayout> startupLabel;
	ComPtr<ID2D1SolidColorBrush> menuBg, border, textBrush, textBrush2,bg;
	float menuW{ 120.f }, menuH{ 38.f }, paddintTop{ 32.f }, menuLeftSpan{ 8.f },borderRadius{6.f};
};

