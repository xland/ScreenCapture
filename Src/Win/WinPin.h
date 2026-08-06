#pragma once
#include <include/Ling.h>

class ToolMain;
class ToolSub;
class WinPin : public Ling::WinBase
{
public:
	~WinPin();
	static void init(int x, int y, int w, int h);
public:
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
private:
	WinPin(int x, int y, int w, int h);
	void onCreated() override;
	void layout() override;
	LRESULT onHitTest(const POINT pos) override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
};

