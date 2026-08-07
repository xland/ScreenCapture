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
	void setPosAndShow();
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
	void initPosSize();
private:
	Ling::Node* contentNode;
	std::vector<Ling::Button*> colorBtns;
	float btnSize{ 32.f };
	float sliderSize{ 80.f };
	float marginTop{ 3.f };
	float arrowX{0.f};
	WinPin* win;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> borderPath;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	bool isVisible{ false };
	bool hasTools{ false };
	UINT selectColorIndex{ 0 };
	std::vector<UINT32> colors = { 0XCF1322FF, 0XD48806FF, 0X389E0DFF, 0X13C2C2FF, 0X0958D9FF, 0X722ED1FF, 0XEB2F96FF, 0X000000FF, 0XFFFFFFFF };
};