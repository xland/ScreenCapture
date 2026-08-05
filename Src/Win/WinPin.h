#pragma once
#include <include/Ling.h>
class WinPin : public Ling::WinBase
{
public:
	~WinPin();
	static void init(int x, int y, int w, int h);
private:
	WinPin(int x, int y, int w, int h);
	void onCreated() override;
	void layout() override;
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
};

