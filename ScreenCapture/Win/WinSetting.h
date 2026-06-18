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
private:
	ComPtr<ID2D1SolidColorBrush> menuBg;
	ComPtr<ID2D1SolidColorBrush> border;
};

