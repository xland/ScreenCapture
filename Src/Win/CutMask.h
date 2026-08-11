#pragma once
#include <include/Ling.h>

// 框选遮罩。WinCap / WinLong 共用：四块半透明遮罩 + 蓝色选区边框 + 左上角尺寸标签。
// 自己不持有绘制目标，paint 时由宿主窗口把当前的 context 传进来。
// 所有坐标都是宿主窗口的客户区坐标。
class CutMask
{
public:
	CutMask(Ling::WinBase* win);
	// 鼠标悬停时吸附到光标下的窗口矩形，选区真的变了才返回 true
	bool highlight(POINT pos);
	void startMakeRect(POINT pos);
	void makeRect(POINT pos);
	void paint(ID2D1DeviceContext* ctx);
public:
	D2D1_RECT_F maskRect{};
	float strokeWidth{ 2.f };
private:
	void initWinRect();
	// 尺寸标签只在 maskRect 变化时重建，不必每帧现建
	void makeLayout();
private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBorder;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushText;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> layout;
	D2D1_RECT_F layoutRect{};
	std::vector<D2D1_RECT_F> winRect;
	POINT pressPos{};
	Ling::WinBase* win{ nullptr };
	float paddingTop{ 3.f };
};
