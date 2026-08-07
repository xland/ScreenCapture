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
	// ToolMain 与 ToolSub 之间的间距，WinPin::layoutTools() 计算整组高度时要用
	static constexpr float mainGap{ 2.f };
private:
	void onCreated() override;
	void layout() override;
	void onMinMaxInfo(MINMAXINFO* mmi);
	void paintBorder(ID2D1DeviceContext* ctx);
	void onColorSelect(Ling::Button* btn);
	void initColorBtns();
	void initSlider();
	// 按内容算出窗口尺寸并应用。btnCount 只数工具按钮，不含颜色按钮。
	// centerOnBtn 为 true 时窗口居中对齐到 ToolMain 上选中的那个按钮，否则与 ToolMain 左对齐。
	void initSize(int btnCount, bool withColors, bool centerOnBtn = false);
private:
	Ling::Node* contentNode;
	std::vector<Ling::Button*> colorBtns;
	float btnSize{ 32.f };
	float sliderSize{ 80.f };     // 与 initSlider 里 setWidth 的值保持一致
	float sliderMargin{ 3.f };     // 滑块左右各留的间距
	float contentInsetX{ 3.f };    // contentNode 的左右内边距之和（onCreated 里 Left 1 + Right 2）
	float marginTop{ 3.f };
	float arrowX{0.f};
	WinPin* win;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> borderPath;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	bool isVisible{ false };
	bool hasTools{ false };
	bool centerOnBtn{ false };
	UINT selectColorIndex{ 0 };
	std::vector<UINT32> colors = { 0XCF1322FF, 0XD48806FF, 0X389E0DFF, 0X13C2C2FF, 0X0958D9FF, 0X722ED1FF, 0XEB2F96FF, 0X000000FF, 0XFFFFFFFF };
	bool isRectFill{ false }, isEllipseFill{ false }, isArrowFill{ true }, isNumberFill{ true }, isLineTransparent{ false }, isTextBold{ false }, isTextItalic{ false }, isMosaicRect{ false }, isEraserRect{ false };
};