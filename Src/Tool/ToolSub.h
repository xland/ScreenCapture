#pragma once
#include <include/Ling.h>

class WinPin;
class ToolSub:public Ling::WinBase
{
public:
	ToolSub(WinPin* win);
	~ToolSub();
	void showRectTools();
	void showEllipseTools();
	void showArrowTools();
	void showNumberTools();
	void showLineTools();
	void showTextTools();
	void showMosaicTools();
	void showEraserTools();

	void hideTools();
	bool hasContent();
	float getDesiredHeight();
	void updatePosition(const RECT& workArea);
	// 当前选中的颜色，直接可喂给 CreateSolidColorBrush。切换工具时颜色按钮会重建，
	// 但选中项由 selectColorIndex 记着，不会丢；mosaic/eraser 没有颜色按钮，返回的是上次选中的颜色。
	D2D1_COLOR_F getSelectedColor() const;
	// 滑块当前值（逻辑像素语义，用作线宽/字号等；D2D 里当物理像素用的话记得乘 dpi）。
	// 值存在 sliderVal 里而不是问 Slider 节点要 —— 节点每次切换工具都被销毁重建。
	float getSliderVal() const;
	// ToolMain 与 ToolSub 之间的间距，WinPin::layoutTools() 计算整组高度时要用
	static constexpr float mainGap{ 2.f };
public:
	bool isRectFill{ false }, isEllipseFill{ false }, isArrowFill{ true }, isNumberFill{ true }, isLineTransparent{ false }, isTextBold{ false }, isTextItalic{ false }, isMosaicRect{ false }, isEraserRect{ false };
private:
	void onCreated() override;
	void layout() override;
	void onMinMaxInfo(MINMAXINFO* mmi);
	void paintBorder(ID2D1DeviceContext* ctx);
	void onColorSelect(Ling::Button* btn);
	void initColorBtns();
	void initSlider();
	// 选中/未选中两套配色，与 ToolMain 的选中效果保持一致
	void applyToggleStyle(Ling::Button* btn, bool selected);
	// 建一个可切换的工具按钮：初始配色跟着 flag，点击时翻转 flag 并刷新配色
	Ling::Button* makeToggleBtn(const std::wstring& text, bool* flag);
	// 按内容算出窗口尺寸并应用。btnCount 只数工具按钮，不含颜色按钮。
	// centerOnBtn 为 true 时窗口居中对齐到 ToolMain 上选中的那个按钮，否则与 ToolMain 左对齐。
	void initSize(int btnCount, bool withColors, bool centerOnBtn = false);
	// 逻辑像素 → 物理像素
	float toPx(float logical) const;
private:
	Ling::Node* contentNode;
	std::vector<Ling::Button*> colorBtns;
	static constexpr float btnSize{ 32.f };
	static constexpr float sliderSize{ 80.f };     // 滑块宽度，initSlider 和 initSize 都用它，改这里就够
	static constexpr float sliderMargin{ 3.f };    // 滑块左右各留的间距
	static constexpr float marginTop{ 3.f };       // 顶部箭头区域高度
	// 边框描边宽度。与 ToolMain 的 setBorder(1.f) 保持一致
	static constexpr float borderW{ 1.f };
	// 箭头尖端相对窗口左边的偏移，由 updatePosition 按屏幕坐标算出，是物理像素
	float arrowX{0.f};
	WinPin* win;
	// 背景/边框画刷缓存：layout() 每次刷新都会调 paintBorder，别在里面重复建
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg, brushBorder;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	bool isVisible{ false };
	bool hasTools{ false };
	bool centerOnBtn{ false };
	UINT selectColorIndex{ 0 };
	// 滑块值。跨工具切换保留，初值取值域中点附近的 2（1px 线太细看不清）。
	float sliderVal{ 2.f };
	float sliderMin{ 1.f }, sliderMax{ 20.f };
	std::vector<UINT32> colors = { 0XCF1322FF, 0XD48806FF, 0X389E0DFF, 0X13C2C2FF, 0X0958D9FF, 0X722ED1FF, 0XEB2F96FF, 0X000000FF, 0XFFFFFFFF };
};