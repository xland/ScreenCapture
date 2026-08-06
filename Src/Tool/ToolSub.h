#pragma once
#include <include/Ling.h>

class WinPin;
class ToolSub:public Ling::WinBase
{
public:
	ToolSub(WinPin* win);
	~ToolSub();
	void setPosAndShow();
private:
	void onCreated() override;
	void layout() override;
	void onMinMaxInfo(MINMAXINFO* mmi);
private:
	float btnSize{ 32.f };
	float sliderSize{ 80.f };
	float marginTop{ 3.f };
	float arrowX{0.f};
	WinPin* win;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> borderPath;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBorder;
	bool isVisible{ false };
};