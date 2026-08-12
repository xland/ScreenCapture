#pragma once
#include <include/Ling.h>

// 光标落在哪一块。选区内部是 Inside，外面被选区四边的延长线切成 8 块，各对应一个方位
enum class MaskHit { None, Inside, Left, Top, Right, Bottom, TopLeft, TopRight, BottomRight, BottomLeft };

// 框选遮罩。四块半透明遮罩 + 蓝色选区边框 + 左上角尺寸标签。
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
	// 选区四边的延长线把窗口切成九块：中间是 Inside，外面八块各是一个方位。没框出选区时才是 None
	MaskHit hitTest(POINT pos) const;
	// 开始调整：记下方向和起始矩形。按的是边或角时，这一下就把那条边吸到光标处
	void startAdjust(POINT pos);
	// 调整中：按 startAdjust 记下的方向改选区
	void adjust(POINT pos);
	// 宽高都大于 0 才算真的框出了东西
	bool hasRect() const;
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
	float paddingTop{ 2.f }, paddingMargin{3.f};
	MaskHit adjustHit{ MaskHit::None };
	// 调整全程以按下那一刻的矩形为基准算，不做累加，免得漂移
	D2D1_RECT_F adjustStartRect{};
	POINT adjustPressPos{};
	// 选区最小尺寸，别让它塌成 0 宽 0 高
	static constexpr float minSize{ 4.f };
};
