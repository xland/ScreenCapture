#pragma once
#include <include/Ling.h>

class ToolMain;
class WinPin : public Ling::WinBase
{
public:
	~WinPin();
	static void init(int x, int y, int w, int h);
private:
	WinPin(int x, int y, int w, int h);
	void onCreated() override;
	void layout() override;
	LRESULT onHitTest(const POINT pos) override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
private:
	std::unique_ptr<ToolMain> toolMain;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
};

